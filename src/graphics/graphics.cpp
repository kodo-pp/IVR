#include <graphics/graphics.hpp>
#include <core/core.hpp>
#include <irrlicht.h>
#include <vector>
#include <string>

#include <unistd.h>

/**
 * Глобальные переменные, хранящие необходимые объекты для работы с Irrlicht
 *
 * Отдельное пространство имён для изоляции
 */
namespace graphics {
    irr::IrrlichtDevice * irrDevice = nullptr;
    irr::video::IVideoDriver * irrVideoDriver = nullptr;
    irr::scene::ISceneManager * irrSceneManager = nullptr;
    irr::gui::IGUIEnvironment * irrGuiEnvironment = nullptr;
    std::vector < std::pair <irr::scene::ISceneNode *,
        irr::scene::IAnimatedMesh *> > graphicalObjects;
}

static bool initializeIrrlicht(std::vector <std::string> * args);
struct FuncResult * handlerGraphicsCreateObject(const std::vector <void *> &);

bool initializeGraphics(std::vector <std::string> * args) {
    if (args == nullptr) {
        return false;
    }

    if (!initializeIrrlicht(args)) {
        return false;
    }

    std::function <struct FuncResult * (const std::vector <void *> &)> handlerFunc(handlerGraphicsCreateObject);
    FuncProvider* prov = new FuncProvider("graphics.createObject", handlerFunc);
    registerFuncProvider(prov);


    // TODO: возможно, вынести код инициализации Irrlicht в отдельныю функцию


    // ***********************************************************************
    // Тест, потом убрать

    FuncProvider* testProv = getFuncProvider("graphics.createObject");
    if (!testProv) {
        throw std::runtime_error("FuncProvider for graphics.createObject was not found");
    }

    std::vector <void *> callArgs;
    std::string modelName = "/home/kodopp/monkey.obj";
    callArgs.push_back(&modelName);
    struct FuncResult * res = (*testProv)(callArgs);

    graphics::irrGuiEnvironment->addStaticText(L"Hello world!", // Сам текст
        irr::core::rect <irr::s32> (10, 10, 260, 260), // Положение текста на экране (прямоугольная область)
        true); // Неясно, что это

    graphics::irrVideoDriver->beginScene(true, // Неясно, что это
        true, // Неясно, что это
        irr::video::SColor(255, 100, 101, 140)); // Какой-то цвет, возможно, цвет фона (ARGB)

    graphics::irrSceneManager->addCameraSceneNode(0, irr::core::vector3df(0,30,-40), irr::core::vector3df(0,5,0));

    graphics::irrGuiEnvironment->drawAll();
    graphics::irrSceneManager->drawAll(); 
    graphics::irrVideoDriver->endScene();
    sleep(3);
    graphics::irrDevice->drop();

    // Конец теста
    // ***********************************************************************
    return true;
}

// Инициаллизация Irrlicht
static bool initializeIrrlicht(std::vector < std::string > * args) {
    graphics::irrDevice = irr::createDevice(
        irr::video::EDT_OPENGL, // Драйвер для рендеринга (здесь OpenGL) (см. http://irrlicht.sourceforge.net/docu/example001.html)
        irr::core::dimension2d <irr::u32> (800, 600), // Размеры окна (не в полноэкранном режиме)
        32, // Глубина цвета
        false, // Полноэкранный режим
        false, // stencil buffer (не очень понятно, что это. COMBAK)
        false, // Вертикальная синхронизация
        0); // Объект-обработчик событий (здесь его нет)
    if (!graphics::irrDevice) {
        // TODO: добавить fallback-настройки
        return false;
    }

    graphics::irrDevice->setWindowCaption(L"Test window"); // COMBAK

    // Получаем необходимые указатели на объекты
    graphics::irrVideoDriver = graphics::irrDevice->getVideoDriver();
    if (!graphics::irrVideoDriver) {
        return false;
    }

    graphics::irrSceneManager = graphics::irrDevice->getSceneManager();
    if (!graphics::irrSceneManager) {
        return false;
    }

    graphics::irrGuiEnvironment = graphics::irrDevice->getGUIEnvironment();
    if (!graphics::irrSceneManager) {
        return false;
    }
    return true;
}

/**
 * Создаёт графический объект
 *
 * Создаёт графический объект: фактически, узел сцены Irrlicht с определёнными
 * параметрами
 *
 * Является функцией для объекта FuncProvider
 * @param args: параметры функции для FuncProvider
 * @param args[0]: Указатель на std::string, содержащий имя файла с 3d-моделью
 *  NOT YET @param args[1]: Указатель на std::string, содержащий имя файла с текстурой
 * @return struct FuncResult *: возвращаемая структура для FuncProvider
 * @return ->data: nullptr при неудаче, индекс графического объекта при успехе
 * @return ->exitStatus: 0 при успехе, 1 при неверных аргументах, 2 при неудачном создании объекта
 */
struct FuncResult * handlerGraphicsCreateObject(const std::vector <void *> & args) {
    if (args.size() != 1) {
        struct FuncResult * result = new struct FuncResult;
        if (!result) {
            throw std::runtime_error("unable to allocate memory to create FuncResult structure");
        }
        result->data = nullptr;
        result->exitStatus = 1;
        return result;
    }
    std::string * modelFileName = (std::string *)args.at(0);
//    std::string * textureFileName = args->at(1);
    if (!modelFileName/* || !textureFileName*/) {
        struct FuncResult * result = new struct FuncResult;
        if (!result) {
            throw std::runtime_error("unable to allocate memory to create FuncResult structure");
        }
        result->data = nullptr;
        result->exitStatus = 1;
        return result;
    }
    irr::scene::IAnimatedMesh * mesh = graphics::irrSceneManager->getMesh(modelFileName->c_str());
    if (!mesh) {
        struct FuncResult * result = new struct FuncResult;
        if (!result) {
            throw std::runtime_error("unable to allocate memory to create FuncResult structure");
        }
        result->data = nullptr;
        result->exitStatus = 2;
        return result;
    }
    irr::scene::ISceneNode * node = graphics::irrSceneManager->addAnimatedMeshSceneNode(mesh);
    if (!node) {
        struct FuncResult * result = new struct FuncResult;
        if (!result) {
            throw std::runtime_error("unable to allocate memory to create FuncResult structure");
        }
        result->data = nullptr;
        result->exitStatus = 2;
        return result;
    }
    struct FuncResult * result = new struct FuncResult;
    if (!result) {
        throw std::runtime_error("unable to allocate memory to create FuncResult structure");
    }
    result->data = nullptr;
    result->exitStatus = 0;
    return result;
}
