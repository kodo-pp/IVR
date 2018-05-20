#include <graphics/graphics.hpp>
#include <core/core.hpp>
#include <log/log.hpp>
#include <iostream>
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

IrrlichtDevice * irrDevice = nullptr;
video::IVideoDriver * irrVideoDriver = nullptr;
scene::ISceneManager * irrSceneManager = nullptr;
gui::IGUIEnvironment * irrGuiEnvironment = nullptr;

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

    //FuncProvider* prov = new FuncProvider(L"graphics.createObject", handlerGraphicsCreateObject);
    //registerFuncProvider(prov);

    /*
    // TODO: возможно, вынести код инициализации Irrlicht в отдельныю функцию


    // ***********************************************************************
    // Тест, потом убрать

    FuncProvider* testProv = getFuncProvider(L"graphics.createObject");
    if (!testProv) {
        throw std::runtime_error("FuncProvider for graphics.createObject was not found");
    }

    std::vector <void *> callArgs;
    std::string modelName = "/home/kodopp/monkey.obj";
    callArgs.push_back(&modelName);
    struct FuncResult * res = (*testProv)(callArgs); // COMBAK: добовить что-то вроде __attribute__((unused))
    irr::scene::ISceneNode * objnode = (irr::scene::ISceneNode*)res->data;
    objnode->setPosition(irr::core::vector3df(20, 0, 0));

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
    int i = 0;
    while (true) {
        ++i;
        graphics::irrVideoDriver->beginScene(true, // Неясно, что это
                                             true, // Неясно, что это
                                             irr::video::SColor(255, 100, 101, 140)); // Какой-то цвет, возможно, цвет фона (ARGB)

        graphics::irrSceneManager->addCameraSceneNode(0, irr::core::vector3df(0,30,-40), irr::core::vector3df(0,5,0));

        graphics::irrGuiEnvironment->drawAll();
        graphics::irrSceneManager->drawAll();
        graphics::irrVideoDriver->endScene();
        usleep(100000);
    }
    sleep(3);
    graphics::irrDevice->drop();

    // Конец теста
    // ***********************************************************************
    */
    return true;
}

// Инициаллизация Irrlicht
static bool initializeIrrlicht(std::vector <std::string> * args) {
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
    graphics::irrDevice->getLogger()->setLogLevel(irr::ELL_NONE);

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

ISceneNode* graphicsCreateObject(const std::wstring& meshFilename) {
    // Load mesh from file
    scene::IAnimatedMesh* mesh = graphics::irrSceneManager->getMesh(meshFilename.c_str());
    if (!mesh) {
        return (ISceneNode*)nullptr;
    }

    // Create a scene node with this mesh
    scene::ISceneNode* node = graphics::irrSceneManager->addAnimatedMeshSceneNode(mesh);
    if (!node) {
        return (ISceneNode*)nullptr;
    }

    node->setMaterialFlag(EMF_LIGHTING, false);

    return node;
}

ISceneNode* graphicsCreateCube() {
    scene::ISceneNode* node = graphics::irrSceneManager->addCubeSceneNode();
    if (!node) {
        return (ISceneNode*)nullptr;
    }

    node->setMaterialFlag(EMF_LIGHTING, false);

    return node;
}

void graphicsDraw() {
    graphics::irrVideoDriver->beginScene(true, // Неясно, что это
                                         true, // Неясно, что это
                                         irr::video::SColor(255, 100, 101, 140)); // Какой-то цвет, возможно, цвет фона (ARGB)

    graphics::irrSceneManager->addCameraSceneNode(0, irr::core::vector3df(0,30,-40), irr::core::vector3df(0,5,0));

    graphics::irrGuiEnvironment->drawAll();
    graphics::irrSceneManager->drawAll();
    graphics::irrVideoDriver->endScene();
}

void graphicsMoveObject(ISceneNode* obj, double x, double y, double z) {
    if (!obj) {
        return;
    }
    obj->setPosition(core::vector3df(x, y, z));
}
void graphicsMoveObject(ISceneNode* obj, core::vector3df pos) {
    if (!obj) {
        return;
    }
    obj->setPosition(pos);
}
void graphicsMoveObject(ISceneNode* obj, GamePosition gp) {
    if (!obj) {
        return;
    }
    obj->setPosition(gp.toIrrVector3df());
}

ITexture* graphicsLoadTexture(std::wstring textureFileName) {
    log(L"loading texture: " << textureFileName);
    ITexture* texture = graphics::irrVideoDriver->getTexture(textureFileName.c_str());
    if (!texture) {
        log(L"Loading texture failed");
        return nullptr;
    }
    log(L"Texture loaded successfully");
    return texture;
}

void graphicsAddTexture(ISceneNode* obj, ITexture* tex) {
    log(L"Adding texture");
    if (!obj || !tex) {
        log(L"Adding texture failed");
        return;
    }
    for (int i = 0; i < 1; ++i) {
        obj->setMaterialTexture(i, tex);
    }
    log(L"Texture added successfully");
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

struct FuncResult * handlerGraphicsCreateObject(const std::vector <void*> & args) {
    // Create the structure which will be returned
    struct FuncResult * result = new struct FuncResult;
    if (!result) {
        throw std::bad_alloc();
    }

    // Parse arguments
    if (args.size() != 1) {
        //result->data = nullptr;
        result->exitStatus = 1;
        return result;
    }
    std::wstring *modelFileName = (std::wstring *)args.at(0);
    if (!modelFileName) {
        //result->data = nullptr;
        result->exitStatus = 1;
        return result;
    }

    ISceneNode *node;

    try {
        node = graphicsCreateObject(*modelFileName);
    } catch (std::bad_alloc& e) {
        //result->data = nullptr;
        result->exitStatus = 2;
        return result;
    }

    result->data.push_back((void *)node);
    result->exitStatus = 0;
    return result;
}
