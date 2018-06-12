#include <graphics/graphics.hpp>
#include <game/objects/objects.hpp>
#include <core/core.hpp>
#include <log/log.hpp>
#include <iostream>
#include <irrlicht.h>
#include <vector>
#include <string>
#include <unistd.h>
#include <modules/module_io.hpp>

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

    std::lock_guard <std::recursive_mutex> lock(gameObjectMutex);

    GameObject* obj = new GameObject(graphicsCreateCube());
    auto objectHandle = registerGameObject(obj);

    setReturn <uint64_t> (ret, 0, objectHandle);
    ret->exitStatus = 0;
    return ret;
}

struct FuncResult* handlerGraphicsMoveObject(const std::vector <void*> & args) {
    if (args.size() != 4) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsMoveObject()");
    }
    auto ret = new struct FuncResult;

    std::lock_guard <std::recursive_mutex> lock(gameObjectMutex);

    uint64_t objectHandle = getArgument <uint64_t> (args, 0);

    double x = getArgument <double> (args, 1);
    double y = getArgument <double> (args, 2);
    double z = getArgument <double> (args, 3);

    graphicsMoveObject(getGameObject(objectHandle)->sceneNode(), GamePosition(x, y, z));

    ret->exitStatus = 0;
    return ret;
}

struct FuncResult* handlerGraphicsDeleteObject(const std::vector <void*> & args) {
    if (args.size() != 1) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsDeleteObject()");
    }
    auto ret = new struct FuncResult;

    std::lock_guard <std::recursive_mutex> lock(gameObjectMutex);

    uint64_t objectHandle = getArgument <uint64_t> (args, 0);

    graphicsDeleteObject(getGameObject(objectHandle));
    unregisterGameObject(objectHandle);

    ret->exitStatus = 0;
    return ret;
}

static inline void initializeGraphicsFuncProviders() {
    registerFuncProvider(new FuncProvider("graphics.createCube", handlerGraphicsCreateCube), "", "L");
    registerFuncProvider(new FuncProvider("graphics.moveObject", handlerGraphicsMoveObject), "LFFF", "");
    registerFuncProvider(new FuncProvider("graphics.deleteObject", handlerGraphicsDeleteObject), "L", "");
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

GameObjCube graphicsCreateCube() {
    std::lock_guard <std::recursive_mutex> lock(gameObjectMutex);
    scene::ISceneNode* node = graphics::irrSceneManager->addCubeSceneNode();
    if (!node) {
        //return (ISceneNode*)nullptr;
        throw std::runtime_error("failed to create a cube scene node");
    }

    node->setMaterialFlag(EMF_LIGHTING, false);

    return GameObjCube(node);
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

void graphicsDeleteObject(GameObject* obj) {
    obj->sceneNode()->remove();
    delete obj;
}

ITexture* graphicsLoadTexture(std::wstring textureFileName) {
    LOG(L"loading texture: " << textureFileName);
    ITexture* texture = graphics::irrVideoDriver->getTexture(textureFileName.c_str());
    if (!texture) {
        LOG(L"Loading texture failed");
        return nullptr;
    }
    LOG(L"Texture loaded successfully");
    return texture;
}

void graphicsAddTexture(const GameObject& obj, ITexture* tex) {
    LOG(L"Adding texture");
    if (!obj.sceneNode() || !tex) {
        LOG(L"Adding texture failed");
        return;
    }
    for (int i = 0; i < 1; ++i) {
        obj.sceneNode()->setMaterialTexture(i, tex);
    }
    LOG(L"Texture added successfully");
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
