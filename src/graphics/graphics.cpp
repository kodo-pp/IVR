#include <graphics/graphics.hpp>
#include <game/objects/objects.hpp>
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
//struct FuncResult * handlerGraphicsCreateObject(const std::vector <void *> &);

struct FuncResult* handlerGraphicsCreateCube(const std::vector <void*> & args) {
    auto ret = new struct FuncResult;
    ret->data.resize(1);

    GameObject* obj = new GameObject(graphicsCreateCube());
    auto objectHandle = registerGameObject(obj);

    log("Created a cube with handle " << objectHandle);

    ret->data.at(0) = (void *)(new uint64_t(objectHandle));
    ret->exitStatus = 0;
    return ret;
}

struct FuncResult* handlerGraphicsMoveObject(const std::vector <void*> & args) {
    if (args.size() != 4) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsMoveObject()");
    }
    auto ret = new struct FuncResult;

    uint64_t objectHandle = *(uint64_t*)(args.at(0));

    // TODO: add floating-point values transportation over network
    int32_t rawX = *(uint32_t*)(args.at(1));
    int32_t rawY = *(uint32_t*)(args.at(2));
    int32_t rawZ = *(uint32_t*)(args.at(3));

    double x = static_cast <double> (rawX) / 1e+6;
    double y = static_cast <double> (rawY) / 1e+6;
    double z = static_cast <double> (rawZ) / 1e+6;

    log("Moving object " << objectHandle << " to (" << x << ", " << y << ", " << z << ")");
    graphicsMoveObject(getGameObject(objectHandle)->sceneNode(), GamePosition(x, y, z));

    ret->exitStatus = 0;
    return ret;
}

static inline void initializeGraphicsFuncProviders() {
    registerFuncProvider(new FuncProvider("graphics.createCube", handlerGraphicsCreateCube), "", "L");
    registerFuncProvider(new FuncProvider("graphics.moveObject", handlerGraphicsMoveObject), "Liii", "");
}

void cleanupGraphics() {
    graphics::irrDevice->drop();
}

bool initializeGraphics(std::vector <std::string> * args) {
    if (args == nullptr) {
        return false;
    }

    if (!initializeIrrlicht(args)) {
        return false;
    }

    initializeGraphicsFuncProviders();

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

GameObjCube&& graphicsCreateCube() {
    scene::ISceneNode* node = graphics::irrSceneManager->addCubeSceneNode();
    if (!node) {
        //return (ISceneNode*)nullptr;
        throw std::runtime_error("failed to create a cube scene node");
    }

    node->setMaterialFlag(EMF_LIGHTING, false);

    return std::move(GameObjCube(node));
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

void graphicsAddTexture(const GameObject& obj, ITexture* tex) {
    log(L"Adding texture");
    if (!obj.sceneNode() || !tex) {
        log(L"Adding texture failed");
        return;
    }
    for (int i = 0; i < 1; ++i) {
        obj.sceneNode()->setMaterialTexture(i, tex);
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

/*
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
*/
