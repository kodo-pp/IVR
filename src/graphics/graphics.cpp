#include <log/log.hpp>
#include <graphics/graphics.hpp>
#include <game/objects/objects.hpp>
#include <core/core.hpp>
#include <iostream>
#include <irrlicht.h>
#include <vector>
#include <string>
#include <unistd.h>
#include <modules/module_io.hpp>
#include <graphics/texture.hpp>
#include <util/util.hpp>
#include <unordered_set>
#include <misc/irrvec.hpp>
#include <geometry/geometry.hpp>
#include <tuple>

bool IrrKeyboardEventReceiver::OnEvent(const irr::SEvent& event) {
    if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
        if (event.KeyInput.PressedDown) {
            pressedKeys.insert(event.KeyInput.Key);
        } else {
            if (pressedKeys.count(event.KeyInput.Key)) {
                pressedKeys.erase(event.KeyInput.Key);
            }
        }
    }
    return false;
}

bool IrrKeyboardEventReceiver::isKeyPressed(irr::EKEY_CODE key) const {
    return pressedKeys.count(key) > 0;
}
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
IrrKeyboardEventReceiver irrEventReceiver;
scene::ICameraSceneNode* camera = nullptr;

//GamePosition cameraPosition(5400, 510, 5200);
GamePosition cameraPosition(0, 30, -40);
core::vector3df cameraRotation(0, 0, 0);

scene::ITerrainSceneNode* terrain = nullptr;

}

static bool initializeIrrlicht(std::vector <std::string> * args);

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

struct FuncResult* handlerGraphicsRotateObject(const std::vector <void*> & args) {
    if (args.size() != 4) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsRotateObject()");
    }

    auto ret = new struct FuncResult;
    std::lock_guard <std::recursive_mutex> lock(gameObjectMutex);

    uint64_t objectHandle = getArgument <uint64_t> (args, 0);

    // TODO: make sure these thing are called this way
    double pitch = getArgument <double> (args, 1);
    double roll = getArgument <double> (args, 2);
    double yaw = getArgument <double> (args, 3);

    graphicsRotateObject(getGameObject(objectHandle)->sceneNode(), core::vector3df(pitch, roll, yaw));

    ret->exitStatus = 0;
    return ret;
}

extern std::recursive_mutex irrlichtMutex;

struct FuncResult* handlerGraphicsLoadTexture(const std::vector <void*> & args) {
    std::lock_guard <std::recursive_mutex> irrlock(irrlichtMutex);
    if (args.size() != 1) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsLoadTexture()");
    }
    auto ret = new struct FuncResult;
    // TODO: replace with wstring

    std::string filename = getArgument <std::string> (args, 0);
    ITexture* texture = graphics::irrVideoDriver->getTexture(filename.c_str());
    ret->data.resize(1);

    if (texture == nullptr) {
        ret->exitStatus = 1;
        setReturn <uint64_t> (ret, 0, 0ULL);
        return ret;
    }

    uint64_t handle = registerTexture(texture);

    ret->exitStatus = 0;
    setReturn <uint64_t> (ret, 0, handle);
    return ret;
}

struct FuncResult* handlerGraphicsAddTexture(const std::vector <void*> & args) {
    if (args.size() != 2) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsAddTexture()");
    }

    auto ret = new struct FuncResult;
    std::lock_guard <std::recursive_mutex> lock(gameObjectMutex);

    uint64_t objectHandle = getArgument <uint64_t> (args, 0);
    uint64_t textureHandle = getArgument <uint64_t> (args, 1);

    GameObject* obj = getGameObject(objectHandle);
    ITexture* texture = accessTexture(textureHandle);

    LOG(L"Adding texture " << textureHandle << L" to object " << objectHandle);

    obj->sceneNode()->setMaterialTexture(0, texture);

    ret->exitStatus = 0;
    return ret;
}

static inline void initializeGraphicsFuncProviders() {
    registerFuncProvider(new FuncProvider("graphics.createCube",           handlerGraphicsCreateCube),   "",     "L");
    registerFuncProvider(new FuncProvider("graphics.moveObject",           handlerGraphicsMoveObject),   "LFFF", "");
    registerFuncProvider(new FuncProvider("graphics.rotateObject",         handlerGraphicsRotateObject), "LFFF", "");
    registerFuncProvider(new FuncProvider("graphics.deleteObject",         handlerGraphicsDeleteObject), "L",    "");
    registerFuncProvider(new FuncProvider("graphics.texture.loadFromFile", handlerGraphicsLoadTexture),  "s",    "L");
    registerFuncProvider(new FuncProvider("graphics.texture.add",          handlerGraphicsAddTexture),   "LL",   "");
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
                              irr::video::EDT_BURNINGSVIDEO, // Драйвер для рендеринга (здесь OpenGL, но пока программный)
                                                             // (см. http://irrlicht.sourceforge.net/docu/example001.html)
                              irr::core::dimension2d <irr::u32> (800, 600), // Размеры окна (не в полноэкранном режиме)
                              32, // Глубина цвета
                              false, // Полноэкранный режим
                              false, // stencil buffer (не очень понятно, что это. COMBAK)
                              false, // Вертикальная синхронизация
                              &graphics::irrEventReceiver); // Объект-обработчик событий
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

    graphics::camera = graphics::irrSceneManager->addCameraSceneNode(0, irr::core::vector3df(0, 30, -40));
    graphics::camera->bindTargetAndRotation(true);
    if (graphics::camera == nullptr) {
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

//    graphics::irrSceneManager->addCameraSceneNode(0, irr::core::vector3df(0,30,-40), irr::core::vector3df(0,5,0));

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

void graphicsRotateObject(ISceneNode* obj, core::vector3df rot) {
    if (!obj) {
        return;
    }
    obj->setRotation(rot);
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

    obj.sceneNode()->setMaterialTexture(0, tex);
    LOG(L"Texture added successfully");
}

static bool checkCameraVerticalOverrotation(const irr::core::vector3df& newRot) {
    LOG(newRot.X << ", " << graphics::cameraRotation.X);
    // Bottom overrotation
    if ((newRot.X + 90) * (graphics::cameraRotation.X + 90) <= 0) {
        return false;
    }
    // Top overrotation
    if ((newRot.X - 90) * (graphics::cameraRotation.X - 90) <= 0) {
        return false;
    }

    return true;
}

static void updateIrrlichtCamera() {
    graphics::camera->setPosition(graphics::cameraPosition.toIrrVector3df());
    graphics::camera->updateAbsolutePosition();
    graphics::camera->setRotation(graphics::cameraRotation);
}

void graphicsMoveCameraTo(const GamePosition& newPos) {
    graphics::cameraPosition = newPos;
    updateIrrlichtCamera();
}
void graphicsMoveCameraDelta(const GamePosition& delta) {
    graphics::cameraPosition += delta;
    updateIrrlichtCamera();
}
void graphicsMoveCameraTo(const core::vector3df& newPos) {
    graphics::cameraPosition = GamePosition(newPos);
    updateIrrlichtCamera();
}
void graphicsMoveCameraDelta(const core::vector3df& delta) {
    graphics::cameraPosition += GamePosition(delta);
    updateIrrlichtCamera();
}
void graphicsMoveCameraTo(double x, double y, double z) {
    graphics::cameraPosition = GamePosition(x, y, z);
    updateIrrlichtCamera();
}
void graphicsMoveCameraDelta(double dx, double dy, double dz) {
    graphics::cameraPosition += GamePosition(dx, dy, dz);
    updateIrrlichtCamera();
}

void graphicsMoveCameraForward(double delta, double directionOffset) {
    // XXX: For now, vertical camera angle is ignored. Maybe it should be so,
    // maybe it should not
    double x, y, z;
    std::tie(x, y, z) = irrvec_extract(graphics::camera->getRotation());
    double azimuth = getAzimuth(x, y, z) + directionOffset;
    double deltaX = delta * sin(azimuth);
    double deltaZ = delta * cos(azimuth);
    graphicsMoveCameraDelta(deltaX, 0.0, deltaZ);
}

void graphicsRotateCamera(const core::vector3df& newRot) {
    graphics::cameraRotation = newRot;
    updateIrrlichtCamera();
}
void graphicsRotateCameraDelta(const core::vector3df& delta) {
    if (!checkCameraVerticalOverrotation(graphics::cameraRotation + delta)) {
        return;
    }
    graphics::cameraRotation += delta;
    updateIrrlichtCamera();
}
void graphicsRotateCamera(double pitch, double roll, double yaw) {
    graphics::cameraRotation = core::vector3df(pitch, roll, yaw);
    updateIrrlichtCamera();
}
void graphicsRotateCameraDelta(double pitch, double roll, double yaw) {
    if (!checkCameraVerticalOverrotation(graphics::cameraRotation + irr::core::vector3df(pitch, roll, yaw))) {
        return;
    }
    graphics::cameraRotation += core::vector3df(pitch, roll, yaw);
    updateIrrlichtCamera();
}

// COMBAK: Stub, maybe customize arguments like node position and scale
// Code taken from http://irrlicht.sourceforge.net/docu/example012.html
void graphicsLoadTerrain(const std::string& heightmap) {
    scene::ITerrainSceneNode* terrain = graphics::irrSceneManager->addTerrainSceneNode(
        heightmap.c_str(),                      // heightmap filename
        nullptr,                                // parent node
        -1,                                     // node id
        // core::vector3df(-5000.0f, -1000.0f, -5000.0f),         // position
        core::vector3df(-500.0f, -300.0f, -500.0f),         // position
        core::vector3df(0.0f, 0.0f, 0.0f),         // rotation
        core::vector3df(1.0f, 1.0f, 1.0f),      // scale
        video::SColor(255, 255, 255, 255),      // vertexColor
        5,                                      // maxLOD
        scene::ETPS_17,                         // patchSize
        4                                       // smoothFactor
    );
    terrain->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    terrain->setMaterialTexture(0, graphicsLoadTexture(L"textures/texture4.png"));
    terrain->scaleTexture(1.0f, 20.0f);
    graphics::terrain = terrain;
}

bool irrDeviceRun() {
    return graphics::irrDevice->run();
}

const IrrKeyboardEventReceiver& getKeyboardEventReceiver() {
    return graphics::irrEventReceiver;
}
