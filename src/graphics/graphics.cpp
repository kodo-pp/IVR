#include <atomic>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

#include <core/core.hpp>
#include <game/objects/objects.hpp>
#include <geometry/geometry.hpp>
#include <graphics/graphics.hpp>
#include <graphics/texture.hpp>
#include <log/log.hpp>
#include <misc/irrvec.hpp>
#include <modules/module_io.hpp>
#include <util/util.hpp>
#include <world/terrain.hpp>

#include <irrlicht.h>
#include <unistd.h>

bool IrrKeyboardEventReceiver::OnEvent(const irr::SEvent& event)
{
    if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
        if (event.KeyInput.PressedDown) {
            pressedKeys.insert(event.KeyInput.Key);
        } else {
            if (pressedKeys.count(event.KeyInput.Key) != 0u) {
                pressedKeys.erase(event.KeyInput.Key);
            }
        }
    }
    return false;
}

bool IrrKeyboardEventReceiver::isKeyPressed(irr::EKEY_CODE key) const
{
    return pressedKeys.count(key) > 0;
}
/**
 * Глобальные переменные, хранящие необходимые объекты для работы с Irrlicht
 *
 * Отдельное пространство имён для изоляции
 */
namespace graphics
{
    IrrlichtDevice* irrDevice = nullptr;
    video::IVideoDriver* irrVideoDriver = nullptr;
    scene::ISceneManager* irrSceneManager = nullptr;
    gui::IGUIEnvironment* irrGuiEnvironment = nullptr;
    IrrKeyboardEventReceiver irrEventReceiver;

    scene::ISceneNode* pseudoCamera = nullptr;
    scene::ICameraSceneNode* camera = nullptr;
    std::map<std::pair<int64_t, int64_t>, scene::ITerrainSceneNode*> terrainChunks;
    scene::ITerrainSceneNode* rootTerrainSceneNode;

    scene::IMetaTriangleSelector* terrainSelector;

    std::atomic<bool> hasCollision(false);

} // namespace graphics

static void initializeIrrlicht(std::vector<std::string>& args);

FuncResult handlerGraphicsCreateCube(UNUSED const std::vector<void*>& args)
{
    FuncResult ret;
    ret.data.resize(1);

    std::lock_guard<std::recursive_mutex> lock(gameObjectMutex);

    GameObject* obj = new GameObject(graphicsCreateCube());
    auto objectHandle = registerGameObject(obj);

    setReturn<uint64_t>(ret, 0, objectHandle);
    return ret;
}

FuncResult handlerGraphicsMoveObject(const std::vector<void*>& args)
{
    if (args.size() != 4) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsMoveObject()");
    }
    FuncResult ret;

    std::lock_guard<std::recursive_mutex> lock(gameObjectMutex);

    uint64_t objectHandle = getArgument<uint64_t>(args, 0);

    double x = getArgument<double>(args, 1);
    double y = getArgument<double>(args, 2);
    double z = getArgument<double>(args, 3);

    graphicsMoveObject(getGameObject(objectHandle)->sceneNode(), GamePosition(x, y, z));

    return ret;
}

FuncResult handlerGraphicsDeleteObject(const std::vector<void*>& args)
{
    if (args.size() != 1) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsDeleteObject()");
    }
    FuncResult ret;

    std::lock_guard<std::recursive_mutex> lock(gameObjectMutex);

    uint64_t objectHandle = getArgument<uint64_t>(args, 0);

    graphicsDeleteObject(getGameObject(objectHandle));
    unregisterGameObject(objectHandle);

    return ret;
}

FuncResult handlerGraphicsRotateObject(const std::vector<void*>& args)
{
    if (args.size() != 4) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsRotateObject()");
    }

    FuncResult ret;
    std::lock_guard<std::recursive_mutex> lock(gameObjectMutex);

    uint64_t objectHandle = getArgument<uint64_t>(args, 0);

    double pitch = getArgument<double>(args, 1);
    double roll = getArgument<double>(args, 2);
    double yaw = getArgument<double>(args, 3);

    graphicsRotateObject(getGameObject(objectHandle)->sceneNode(),
                         core::vector3df(pitch, roll, yaw));

    return ret;
}

extern std::recursive_mutex irrlichtMutex;

FuncResult handlerGraphicsLoadTexture(const std::vector<void*>& args)
{
    std::lock_guard<std::recursive_mutex> irrlock(irrlichtMutex);
    if (args.size() != 1) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsLoadTexture()");
    }
    FuncResult ret;
    // TODO: replace with wstring

    std::string filename = getArgument<std::string>(args, 0);
    ITexture* texture = graphics::irrVideoDriver->getTexture(filename.c_str());
    ret.data.resize(1);

    if (texture == nullptr) {
        setReturn<uint64_t>(ret, 0, 0ULL);
        return ret;
    }

    uint64_t handle = registerTexture(texture);

    setReturn<uint64_t>(ret, 0, handle);
    return ret;
}

FuncResult handlerGraphicsAddTexture(const std::vector<void*>& args)
{
    if (args.size() != 2) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsAddTexture()");
    }

    FuncResult ret;
    std::lock_guard<std::recursive_mutex> lock(gameObjectMutex);

    uint64_t objectHandle = getArgument<uint64_t>(args, 0);
    uint64_t textureHandle = getArgument<uint64_t>(args, 1);

    GameObject* obj = getGameObject(objectHandle);
    ITexture* texture = accessTexture(textureHandle);

    LOG(L"Adding texture " << textureHandle << L" to object " << objectHandle);

    obj->sceneNode()->setMaterialTexture(0, texture);

    return ret;
}

FuncResult handlerGraphicsDrawableAddTexture(const std::vector<void*>& args)
{
    if (args.size() != 2) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsAddTexture()");
    }

    FuncResult ret;
    std::lock_guard<std::recursive_mutex> lock(gameObjectMutex);

    uint64_t objectHandle = getArgument<uint64_t>(args, 0);
    uint64_t textureHandle = getArgument<uint64_t>(args, 1);

    auto obj = drawablesManager.access(objectHandle);
    ITexture* texture = accessTexture(textureHandle);

    LOG(L"Adding texture " << textureHandle << L" to object " << objectHandle);

    obj->setMaterialTexture(0, texture);

    return ret;
}

scene::ISceneNode* graphicsCreateDrawableCube();

FuncResult handlerCreateDrawableCube(const std::vector<void*>& args)
{
    if (args.size() != 0) {
        throw std::logic_error("Wrong number of arguments for handlerCreateDrawableCube()");
    }

    FuncResult ret;
    ret.data.resize(1);

    LOG(L"Creating drawable cube");
    setReturn<uint64_t>(ret, 0, drawablesManager.track(graphicsCreateDrawableCube()));
    return ret;
}

FuncResult handlerDrawableEnablePhysics(const std::vector<void*>& args)
{
    if (args.size() != 4) {
        throw std::logic_error("Wrong number of arguments for handlerDrawableEnablePhysics()");
    }

    auto drawableHandle = getArgument<uint64_t>(args, 0);
    auto x = getArgument<float>(args, 1);
    auto y = getArgument<float>(args, 2);
    auto z = getArgument<float>(args, 3);

    auto drawable = drawablesManager.access(drawableHandle);

    graphicsEnablePhysics(drawable, {x, y, z});

    FuncResult ret;
    return ret;
}

static inline void initializeGraphicsFuncProviders()
{
    registerFuncProvider(FuncProvider("graphics.createCube", handlerGraphicsCreateCube), "", "L");
    registerFuncProvider(
            FuncProvider("graphics.moveObject", handlerGraphicsMoveObject), "LFFF", "");
    registerFuncProvider(
            FuncProvider("graphics.rotateObject", handlerGraphicsRotateObject), "LFFF", "");
    registerFuncProvider(
            FuncProvider("graphics.deleteObject", handlerGraphicsDeleteObject), "L", "");
    registerFuncProvider(
            FuncProvider("graphics.texture.loadFromFile", handlerGraphicsLoadTexture), "s", "L");
    registerFuncProvider(FuncProvider("graphics.texture.add", handlerGraphicsAddTexture), "LL", "");
    registerFuncProvider(
            FuncProvider("graphics.texture.addToDrawable", handlerGraphicsDrawableAddTexture),
            "LL",
            "");
    registerFuncProvider(
            FuncProvider("graphics.drawable.createCube", handlerCreateDrawableCube), "", "L");
    registerFuncProvider(
            FuncProvider("graphics.drawable.enablePhysics", handlerDrawableEnablePhysics),
            "Lfff",
            "");
}

void cleanupGraphics()
{
    graphics::terrainSelector->drop();
    graphics::irrDevice->drop();
}

void initializeGraphics(std::vector<std::string>& args)
{
    initializeIrrlicht(args);
    initializeGraphicsFuncProviders();
}

// Инициаллизация Irrlicht
static void initializeIrrlicht(UNUSED std::vector<std::string>& args)
{
    graphics::irrDevice = irr::createDevice(
            irr::video::EDT_SOFTWARE, // Драйвер для рендеринга (здесь OpenGL, но пока
                                      // программный)
            // (см. http://irrlicht.sourceforge.net/docu/example001.html)
            irr::core::dimension2d<irr::u32>(800,
                                             600), // Размеры окна (не в полноэкранном режиме)
            32,                                    // Глубина цвета
            false,                                 // Полноэкранный режим
            false, // stencil buffer (не очень понятно, что это. COMBAK)
            true,  // Вертикальная синхронизация
            &graphics::irrEventReceiver); // Объект-обработчик событий
    if (graphics::irrDevice == nullptr) {
        // TODO: добавить fallback-настройки
        throw std::runtime_error("Failed to initialize Irrlicht device");
    }
    graphics::irrDevice->getLogger()->setLogLevel(irr::ELL_NONE);

    graphics::irrDevice->setWindowCaption(L"Test window"); // COMBAK

    // Получаем необходимые указатели на объекты
    graphics::irrVideoDriver = graphics::irrDevice->getVideoDriver();
    if (graphics::irrVideoDriver == nullptr) {
        throw std::runtime_error("Failed to access Irrlicht video driver");
    }

    graphics::irrSceneManager = graphics::irrDevice->getSceneManager();
    if (graphics::irrSceneManager == nullptr) {
        throw std::runtime_error("Failed to access Irrlicht scene manager");
    }

    graphics::irrGuiEnvironment = graphics::irrDevice->getGUIEnvironment();
    if (graphics::irrGuiEnvironment == nullptr) {
        throw std::runtime_error("Failed to access Irrlicht GUI environment");
    }

    graphics::camera = graphics::irrSceneManager->addCameraSceneNode(
            0, irr::core::vector3df(0, 30, -40));
    if (graphics::camera == nullptr) {
        throw std::runtime_error("Failed to create Irrlicht camera");
    }
    graphics::camera->bindTargetAndRotation(true);
    graphics::pseudoCamera = graphics::irrSceneManager->addEmptySceneNode();
    if (graphics::pseudoCamera == nullptr) {
        throw std::runtime_error("Failed to create Irrlicht pseudo-camera");
    }
    graphics::pseudoCamera->setPosition(graphics::camera->getPosition());

    // graphicsInitializeCollisions();
}

GameObjCube graphicsCreateCube()
{
    std::lock_guard<std::recursive_mutex> lock(gameObjectMutex);
    scene::ISceneNode* node = graphics::irrSceneManager->addCubeSceneNode();
    if (node == nullptr) {
        // return (ISceneNode*)nullptr;
        throw std::runtime_error("failed to create a cube scene node");
    }

    node->setMaterialFlag(EMF_LIGHTING, false);

    return GameObjCube(node);
}

void graphicsDraw()
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
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

void graphicsMoveObject(ISceneNode* obj, double x, double y, double z)
{
    if (obj == nullptr) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);

    obj->setPosition(core::vector3df(x, y, z));
}
void graphicsMoveObject(ISceneNode* obj, const core::vector3df& pos)
{
    if (obj == nullptr) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    obj->setPosition(pos);
}
void graphicsMoveObject(ISceneNode* obj, const GamePosition& gp)
{
    if (obj == nullptr) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    obj->setPosition(gp.toIrrVector3df());
}

void graphicsDeleteObject(GameObject* obj)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    obj->sceneNode()->remove();
    delete obj;
}

void graphicsRotateObject(ISceneNode* obj, const core::vector3df& rot)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    if (obj == nullptr) {
        return;
    }
    obj->setRotation(rot);
}

ITexture* graphicsLoadTexture(const std::wstring& textureFileName)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    LOG(L"loading texture: " << textureFileName);
    ITexture* texture = graphics::irrVideoDriver->getTexture(textureFileName.c_str());
    if (texture == nullptr) {
        LOG(L"Loading texture failed");
        return nullptr;
    }
    LOG(L"Texture loaded successfully");
    return texture;
}

void graphicsAddTexture(const GameObject& obj, ITexture* tex)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    LOG(L"Adding texture");
    if ((obj.sceneNode() == nullptr) || (tex == nullptr)) {
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
                         video::ITexture* detail)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
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
    if (terrain == nullptr) {
        throw std::runtime_error("unable to create terrain scene node");
    }
    terrain->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    terrain->setMaterialTexture(1, tex);
    terrain->setMaterialTexture(0, detail);
    terrain->scaleTexture(1.0f, 20.0f);

    Chunk terrainChunk({}, terrain);
    terrainManager.addChunk(off_x, off_y, std::move(terrainChunk));
}

void graphicsHandleCollisions(scene::ITerrainSceneNode* node)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto selector = graphics::irrSceneManager->createTerrainTriangleSelector(node);
    if (selector == nullptr) {
        throw std::runtime_error("unable to create triangle selector on terrain scene node");
    }

    static_cast<scene::IMetaTriangleSelector*>(
            static_cast<scene::ISceneNodeAnimatorCollisionResponse*>(
                    *graphics::pseudoCamera->getAnimators().begin())
                    ->getWorld())
            ->addTriangleSelector(selector);
    selector->drop();
}

void graphicsHandleCollisionsMesh(scene::IMesh* mesh, scene::ISceneNode* node)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto selector = graphics::irrSceneManager->createTriangleSelector(mesh, node);
    if (selector == nullptr) {
        throw std::runtime_error("unable to create triangle selector on mesh scene node");
    }

    static_cast<scene::IMetaTriangleSelector*>(
            static_cast<scene::ISceneNodeAnimatorCollisionResponse*>(
                    *graphics::pseudoCamera->getAnimators().begin())
                    ->getWorld())
            ->addTriangleSelector(selector);
    selector->drop();
}

void graphicsHandleCollisionsBoundingBox(scene::ISceneNode* node)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto selector = graphics::irrSceneManager->createTriangleSelectorFromBoundingBox(node);
    if (selector == nullptr) {
        throw std::runtime_error("unable to create triangle selector on scene node bounding box");
    }

    static_cast<scene::IMetaTriangleSelector*>(
            static_cast<scene::ISceneNodeAnimatorCollisionResponse*>(
                    *graphics::pseudoCamera->getAnimators().begin())
                    ->getWorld())
            ->addTriangleSelector(selector);
    selector->drop();
}

void graphicsEnablePhysics(scene::ISceneNode* node, const core::vector3df& radius)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
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

void graphicsDisablePhysics(scene::ISceneNode* node)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    node->removeAnimators();
}

void graphicsInitializeCollisions()
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto selector = graphics::irrSceneManager->createMetaTriangleSelector();
    if (selector == nullptr) {
        throw std::runtime_error("unable to create meta triangle selector");
    }

    auto animator = graphics::irrSceneManager->createCollisionResponseAnimator(
            selector,                    // Triangle selector
            graphics::pseudoCamera,      // Affected scene node
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

    graphics::pseudoCamera->addAnimator(animator);
    animator->drop();
}

irr::scene::ICameraSceneNode* graphicsGetCamera()
{
    return graphics::camera;
}

irr::scene::ISceneNode* graphicsGetPseudoCamera()
{
    return graphics::pseudoCamera;
}

bool irrDeviceRun()
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    return graphics::irrDevice->run();
}

const IrrKeyboardEventReceiver& getKeyboardEventReceiver()
{
    return graphics::irrEventReceiver;
}

std::pair<bool, GamePosition> graphicsGetPlacePosition(const GamePosition& pos,
                                                       const GamePosition& target)
{
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

scene::ISceneNode* graphicsCreateMeshSceneNode(scene::IMesh* mesh)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto node = graphics::irrSceneManager->addMeshSceneNode(mesh);
    if (node == nullptr) {
        throw std::runtime_error("unable to create mesh scene node");
    }
    return node;
}

scene::IMesh* graphicsLoadMesh(const std::wstring& filename)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto mesh = graphics::irrSceneManager->getMesh(filename.c_str());
    if (mesh == nullptr) {
        throw std::runtime_error("unable to load mesh from file");
    }
    return mesh;
}

scene::ISceneNode* graphicsCreateDrawableCube()
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto node = graphics::irrSceneManager->addCubeSceneNode();
    if (node == nullptr) {
        throw std::runtime_error("unable to add cube scene node");
    }
    return node;
}

void graphicsJump(scene::ISceneNode* node, float jumpSpeed)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto list = node->getAnimators();
    if (list.empty()) {
        throw std::runtime_error("Physics are disabled for this scene node");
    }

    auto animator = static_cast<scene::ISceneNodeAnimatorCollisionResponse*>(*list.begin());
    if (!animator->isFalling()) {
        animator->jump(jumpSpeed);
    }
}

void graphicsStep(scene::ISceneNode* node, float distance)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto direction = node->getRotation().rotationToDirection().normalize();
    node->setPosition(node->getPosition() + direction * distance);
}

void graphicsLookAt(scene::ISceneNode* node, float x, float y, float z)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    core::vector3df src = node->getAbsolutePosition();
    core::vector3df dst(x, y, z);
    core::vector3df diff = dst - src;
    node->setRotation(diff.getHorizontalAngle());
}

void graphicsGetPosition(scene::ISceneNode* node, float& x, float& y, float& z)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    std::array<float, 3> arr;
    node->getPosition().getAs3Values(arr.data());
    x = arr[0];
    y = arr[1];
    z = arr[2];
}

irr::gui::IGUIListBox* createListBox(const std::vector<std::wstring>& strings,
                                     const core::recti& position)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto listbox = graphics::irrGuiEnvironment->addListBox(position);
    for (const auto& s : strings) {
        listbox->addItem(s.c_str());
    }
    return listbox;
}
