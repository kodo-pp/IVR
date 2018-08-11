#include <atomic>
#include <core/core.hpp>
#include <game/objects/objects.hpp>
#include <geometry/geometry.hpp>
#include <graphics/graphics.hpp>
#include <graphics/texture.hpp>
#include <iostream>
#include <irrlicht.h>
#include <log/log.hpp>
#include <map>
#include <misc/irrvec.hpp>
#include <modules/module_io.hpp>
#include <string>
#include <tuple>
#include <unistd.h>
#include <unordered_set>
#include <util/util.hpp>
#include <vector>
#include <world/terrain.hpp>

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

IrrlichtDevice* irrDevice = nullptr;
video::IVideoDriver* irrVideoDriver = nullptr;
scene::ISceneManager* irrSceneManager = nullptr;
gui::IGUIEnvironment* irrGuiEnvironment = nullptr;
IrrKeyboardEventReceiver irrEventReceiver;

scene::ICameraSceneNode* camera = nullptr;
std::map<std::pair<int64_t, int64_t>, scene::ITerrainSceneNode*> terrainChunks;
scene::ITerrainSceneNode* rootTerrainSceneNode;

scene::IMetaTriangleSelector* terrainSelector;

std::atomic<bool> hasCollision(false);

} // namespace graphics

static bool initializeIrrlicht(std::vector<std::string>* args);

struct FuncResult* handlerGraphicsCreateCube(UNUSED const std::vector<void*>& args) {
    auto ret = new struct FuncResult;
    ret->data.resize(1);

    std::lock_guard<std::recursive_mutex> lock(gameObjectMutex);

    GameObject* obj = new GameObject(graphicsCreateCube());
    auto objectHandle = registerGameObject(obj);

    setReturn<uint64_t>(ret, 0, objectHandle);
    ret->exitStatus = 0;
    return ret;
}

struct FuncResult* handlerGraphicsMoveObject(const std::vector<void*>& args) {
    if (args.size() != 4) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsMoveObject()");
    }
    auto ret = new struct FuncResult;

    std::lock_guard<std::recursive_mutex> lock(gameObjectMutex);

    uint64_t objectHandle = getArgument<uint64_t>(args, 0);

    double x = getArgument<double>(args, 1);
    double y = getArgument<double>(args, 2);
    double z = getArgument<double>(args, 3);

    graphicsMoveObject(getGameObject(objectHandle)->sceneNode(), GamePosition(x, y, z));

    ret->exitStatus = 0;
    return ret;
}

struct FuncResult* handlerGraphicsDeleteObject(const std::vector<void*>& args) {
    if (args.size() != 1) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsDeleteObject()");
    }
    auto ret = new struct FuncResult;

    std::lock_guard<std::recursive_mutex> lock(gameObjectMutex);

    uint64_t objectHandle = getArgument<uint64_t>(args, 0);

    graphicsDeleteObject(getGameObject(objectHandle));
    unregisterGameObject(objectHandle);

    ret->exitStatus = 0;
    return ret;
}

struct FuncResult* handlerGraphicsRotateObject(const std::vector<void*>& args) {
    if (args.size() != 4) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsRotateObject()");
    }

    auto ret = new struct FuncResult;
    std::lock_guard<std::recursive_mutex> lock(gameObjectMutex);

    uint64_t objectHandle = getArgument<uint64_t>(args, 0);

    // TODO: make sure these thing are called this way
    double pitch = getArgument<double>(args, 1);
    double roll = getArgument<double>(args, 2);
    double yaw = getArgument<double>(args, 3);

    graphicsRotateObject(getGameObject(objectHandle)->sceneNode(),
                         core::vector3df(pitch, roll, yaw));

    ret->exitStatus = 0;
    return ret;
}

extern std::recursive_mutex irrlichtMutex;

FuncResult* handlerGraphicsLoadTexture(const std::vector<void*>& args) {
    std::lock_guard<std::recursive_mutex> irrlock(irrlichtMutex);
    if (args.size() != 1) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsLoadTexture()");
    }
    auto ret = new struct FuncResult;
    // TODO: replace with wstring

    std::string filename = getArgument<std::string>(args, 0);
    ITexture* texture = graphics::irrVideoDriver->getTexture(filename.c_str());
    ret->data.resize(1);

    if (texture == nullptr) {
        ret->exitStatus = 1;
        setReturn<uint64_t>(ret, 0, 0ULL);
        return ret;
    }

    uint64_t handle = registerTexture(texture);

    ret->exitStatus = 0;
    setReturn<uint64_t>(ret, 0, handle);
    return ret;
}

struct FuncResult* handlerGraphicsAddTexture(const std::vector<void*>& args) {
    if (args.size() != 2) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsAddTexture()");
    }

    auto ret = new struct FuncResult;
    std::lock_guard<std::recursive_mutex> lock(gameObjectMutex);

    uint64_t objectHandle = getArgument<uint64_t>(args, 0);
    uint64_t textureHandle = getArgument<uint64_t>(args, 1);

    GameObject* obj = getGameObject(objectHandle);
    ITexture* texture = accessTexture(textureHandle);

    LOG(L"Adding texture " << textureHandle << L" to object " << objectHandle);

    obj->sceneNode()->setMaterialTexture(0, texture);

    ret->exitStatus = 0;
    return ret;
}

static inline void initializeGraphicsFuncProviders() {
    registerFuncProvider(
            new FuncProvider("graphics.createCube", handlerGraphicsCreateCube), "", "L");
    registerFuncProvider(
            new FuncProvider("graphics.moveObject", handlerGraphicsMoveObject), "LFFF", "");
    registerFuncProvider(
            new FuncProvider("graphics.rotateObject", handlerGraphicsRotateObject), "LFFF", "");
    registerFuncProvider(
            new FuncProvider("graphics.deleteObject", handlerGraphicsDeleteObject), "L", "");
    registerFuncProvider(
            new FuncProvider("graphics.texture.loadFromFile", handlerGraphicsLoadTexture),
            "s",
            "L");
    registerFuncProvider(
            new FuncProvider("graphics.texture.add", handlerGraphicsAddTexture), "LL", "");
}

void cleanupGraphics() {
    graphics::terrainSelector->drop();
    graphics::irrDevice->drop();
}

bool initializeGraphics(std::vector<std::string>* args) {
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
static bool initializeIrrlicht(UNUSED std::vector<std::string>* args) {
    graphics::irrDevice = irr::createDevice(
            irr::video::EDT_BURNINGSVIDEO, // Драйвер для рендеринга (здесь OpenGL, но пока
                                           // программный)
            // (см. http://irrlicht.sourceforge.net/docu/example001.html)
            irr::core::dimension2d<irr::u32>(800,
                                             600), // Размеры окна (не в полноэкранном режиме)
            32,                                    // Глубина цвета
            false,                                 // Полноэкранный режим
            false, // stencil buffer (не очень понятно, что это. COMBAK)
            true,  // Вертикальная синхронизация
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

    graphics::camera = graphics::irrSceneManager->addCameraSceneNode(
            0, irr::core::vector3df(0, 30, -40));
    if (graphics::camera == nullptr) {
        return false;
    }
    graphics::camera->bindTargetAndRotation(true);

    graphicsInitializeCollisions();
    return true;
}

GameObjCube graphicsCreateCube() {
    std::lock_guard<std::recursive_mutex> lock(gameObjectMutex);
    scene::ISceneNode* node = graphics::irrSceneManager->addCubeSceneNode();
    if (!node) {
        // return (ISceneNode*)nullptr;
        throw std::runtime_error("failed to create a cube scene node");
    }

    node->setMaterialFlag(EMF_LIGHTING, false);

    return GameObjCube(node);
}

void graphicsDraw() {
    graphics::irrVideoDriver->beginScene(
            true, // Неясно, что это
            true, // Неясно, что это
            irr::video::SColor(255,
                               100,
                               101,
                               140)); // Какой-то цвет, возможно, цвет фона (ARGB)

    //    graphics::irrSceneManager->addCameraSceneNode(0, irr::core::vector3df(0,30,-40),
    //    irr::core::vector3df(0,5,0));

    graphics::irrGuiEnvironment->drawAll();
    graphics::irrSceneManager->drawAll();
    core::recti viewport = graphics::irrVideoDriver->getViewPort();
    auto lt = viewport.getCenter() - irr::core::vector2di(10, 10);
    auto rb = viewport.getCenter() + irr::core::vector2di(10);
    if (graphics::hasCollision) {
        graphics::irrVideoDriver->draw2DRectangle(video::SColor(180, 0, 255, 0),
                                                  core::recti(lt, rb));
    } else {
        graphics::irrVideoDriver->draw2DRectangle(video::SColor(180, 255, 0, 0),
                                                  core::recti(lt, rb));
    }
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

// COMBAK: Stub, maybe customize arguments like node position and scale
// Code taken from http://irrlicht.sourceforge.net/docu/example012.html

void graphicsLoadTerrain(int64_t off_x,
                         int64_t off_y,
                         const std::wstring& heightmap,
                         video::ITexture* tex,
                         video::ITexture* detail) {
    double irrOffsetX = CHUNK_SIZE_IRRLICHT * off_x;
    double irrOffsetY = CHUNK_SIZE_IRRLICHT * off_y;
    scene::ITerrainSceneNode* terrain = graphics::irrSceneManager->addTerrainSceneNode(
            heightmap.c_str(),                                         // heightmap filename
            nullptr,                                                   // parent node
            -1,                                                        // node id
            core::vector3df(irrOffsetX - 180, -600, irrOffsetY - 200), // position
            core::vector3df(0.0f, 0.0f, 0.0f),                         // rotation
            core::vector3df(10.0f, 4.0f, 10.0f),                       // scale
            video::SColor(255, 255, 255, 255),                         // vertexColor (?)
            5,                                                         // maxLOD (Level Of Detail)
            scene::ETPS_17,                                            // patchSize (?)
            4                                                          // smoothFactor (?)
    );
    if (!terrain) {
        throw std::runtime_error("unable to create terrain scene node");
    }
    terrain->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    terrain->setMaterialTexture(1, tex);
    terrain->setMaterialTexture(0, detail);
    terrain->scaleTexture(1.0f, 20.0f);

    TerrainChunk terrainChunk(terrain);
    terrainManager.addChunk(off_x, off_y, std::move(terrainChunk));
}

void graphicsHandleCollisions(scene::ITerrainSceneNode* node) {
    auto selector = graphics::irrSceneManager->createTerrainTriangleSelector(node);
    if (selector == nullptr) {
        throw std::runtime_error("unable to create triangle selector on terrain scene node");
    }

    static_cast<scene::IMetaTriangleSelector*>(
            static_cast<scene::ISceneNodeAnimatorCollisionResponse*>(
                    *graphics::camera->getAnimators().begin())
                    ->getWorld())
            ->addTriangleSelector(selector);
    selector->drop();
}

void graphicsHandleCollisionsMesh(scene::IMesh* mesh, scene::ISceneNode* node) {
    auto selector = graphics::irrSceneManager->createTriangleSelector(mesh, node);
    if (selector == nullptr) {
        throw std::runtime_error("unable to create triangle selector on mesh scene node");
    }

    static_cast<scene::IMetaTriangleSelector*>(
            static_cast<scene::ISceneNodeAnimatorCollisionResponse*>(
                    *graphics::camera->getAnimators().begin())
                    ->getWorld())
            ->addTriangleSelector(selector);
    selector->drop();
}

void graphicsHandleCollisionsBoundingBox(scene::ISceneNode* node) {
    auto selector = graphics::irrSceneManager->createTriangleSelectorFromBoundingBox(node);
    if (selector == nullptr) {
        throw std::runtime_error("unable to create triangle selector on scene node bounding box");
    }

    static_cast<scene::IMetaTriangleSelector*>(
            static_cast<scene::ISceneNodeAnimatorCollisionResponse*>(
                    *graphics::camera->getAnimators().begin())
                    ->getWorld())
            ->addTriangleSelector(selector);
    selector->drop();
}

void graphicsEnablePhysics(scene::ISceneNode* node, const core::vector3df& radius) {
    auto animator = graphics::irrSceneManager->createCollisionResponseAnimator(
            graphics::terrainSelector, // Comment to make code autoformatter happy
            node,
            radius,
            core::vector3df(0, -20, 0),
            core::vector3df(0, 0, 0),
            0);
    if (animator == nullptr) {
        throw std::runtime_error("unable to create collision response animator for object");
    }
    node->addAnimator(animator);
    animator->drop();
}

void graphicsDisablePhysics(scene::ISceneNode* node) {
    node->removeAnimators();
}

void graphicsInitializeCollisions() {
    auto selector = graphics::irrSceneManager->createMetaTriangleSelector();
    if (selector == nullptr) {
        throw std::runtime_error("unable to create meta triangle selector");
    }

    auto animator = graphics::irrSceneManager->createCollisionResponseAnimator(
            selector,                    // Triangle selector
            graphics::camera,            // Affected scene node
            core::vector3df(30, 60, 30), // Collision radius
            core::vector3df(0, -20, 0),  // Gravity vector
            core::vector3df(0, 30, 0),   // Ellipsoid translation
            0.000f                       // Sliding value
    );
    graphics::terrainSelector = selector;
    if (animator == nullptr) {
        throw std::runtime_error(
                "unable to create camera collision animator for terrain scene node");
    }

    graphics::camera->addAnimator(animator);
    animator->drop();
}

irr::scene::ICameraSceneNode* graphicsGetCamera() {
    return graphics::camera;
}

bool irrDeviceRun() {
    return graphics::irrDevice->run();
}

const IrrKeyboardEventReceiver& getKeyboardEventReceiver() {
    return graphics::irrEventReceiver;
}

std::pair<bool, GamePosition> graphicsGetPlacePosition(const GamePosition& pos,
                                                       const GamePosition& target) {
    // Thanks to irrlicht.sourceforge.net/docu/example007.html
    core::line3df ray;
    ray.start = pos.toIrrVector3df();
    ray.end = ray.start + (target.toIrrVector3df() - ray.start).normalize() * 450;
    auto collisionManager = graphics::irrSceneManager->getSceneCollisionManager();

    core::vector3df hitPoint;
    UNUSED core::triangle3df dummy1;
    UNUSED scene::ISceneNode* dummy2;

    bool collisionHappened = collisionManager->getCollisionPoint(
            ray,                       // A comment to make autoformatter happy
            graphics::terrainSelector, // Even more happy -------
            hitPoint,
            dummy1,
            dummy2 // EVEN MORE I SAID
    );

    graphics::hasCollision = collisionHappened;
    return {collisionHappened, GamePosition(hitPoint)};
}

scene::ISceneNode* graphicsCreateMeshSceneNode(scene::IMesh* mesh) {
    auto node = graphics::irrSceneManager->addMeshSceneNode(mesh);
    if (node == nullptr) {
        throw std::runtime_error("unable to create mesh scene node");
    }
    return node;
}

scene::IMesh* graphicsLoadMesh(const std::wstring& filename) {
    auto mesh = graphics::irrSceneManager->getMesh(filename.c_str());
    if (mesh == nullptr) {
        throw std::runtime_error("unable to load mesh from file");
    }
    return mesh;
}
