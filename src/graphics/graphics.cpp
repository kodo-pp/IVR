#include <atomic>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

#include <modbox/core/core.hpp>
#include <modbox/game/game_loop.hpp>
#include <modbox/game/objects/objects.hpp>
#include <modbox/geometry/geometry.hpp>
#include <modbox/graphics/graphics.hpp>
#include <modbox/graphics/texture.hpp>
#include <modbox/log/log.hpp>
#include <modbox/misc/irrvec.hpp>
#include <modbox/modules/module_io.hpp>
#include <modbox/util/util.hpp>
#include <modbox/world/terrain.hpp>

#include <irrlicht.h>
#include <unistd.h>

// Глобальные переменные, хранящие необходимые объекты для работы с Irrlicht
// Отдельное пространство имён для изоляции
// По идее, всё это надо убрать в класс, но лень
namespace graphics
{
    IrrlichtDevice* irrDevice = nullptr;
    video::IVideoDriver* irrVideoDriver = nullptr;
    scene::ISceneManager* irrSceneManager = nullptr;
    gui::IGUIEnvironment* irrGuiEnvironment = nullptr;
    IrrEventReceiver irrEventReceiver;

    scene::ISceneNode* pseudoCamera = nullptr;
    scene::ICameraSceneNode* camera = nullptr;
    std::map<std::pair<int64_t, int64_t>, scene::ITerrainSceneNode*> terrainChunks;
    scene::ITerrainSceneNode* rootTerrainSceneNode;

    scene::IMetaTriangleSelector* terrainSelector;

    std::atomic<bool> hasCollision(false);
    std::atomic<bool> aimVisible(false);

    std::unordered_map<std::string, irr::video::ITexture*> textureCache;

    HandleStorage<uint64_t, std::pair<irr::core::rectf, irr::video::SColor>> rectangles;
    HandleStorage<uint64_t, std::pair<irr::core::line2df, irr::video::SColor>> lines;
    HandleStorage<uint64_t, std::pair<irr::core::rectf, irr::video::ITexture*>> images;
} // namespace graphics

extern std::recursive_mutex irrlichtMutex;

// При вызове текущий поток блокируется до тех пор, пока все задачи основному потоку не будут
// завершены
void drawBarrier()
{
    addDrawFunction([]() -> void { LOG("--- Draw barrier ---"); }, true);
}

// Обработчик событий графического движка
bool IrrEventReceiver::OnEvent(const irr::SEvent& event)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    std::lock_guard<std::recursive_mutex> lock2(irrlichtMutex);
    if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
        if (event.KeyInput.PressedDown) {
            pressedKeys.insert(event.KeyInput.Key);
        } else {
            if (pressedKeys.count(event.KeyInput.Key) != 0u) {
                pressedKeys.erase(event.KeyInput.Key);
            }
        }
    } else if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        if (event.MouseInput.isLeftPressed()) {
            leftMouseButtonDown = true;
        } else {
            leftMouseButtonDown = false;
        }
        if (event.MouseInput.isMiddlePressed()) {
            middleMouseButtonDown = true;
        } else {
            middleMouseButtonDown = false;
        }
        if (event.MouseInput.isRightPressed()) {
            rightMouseButtonDown = true;
        } else {
            rightMouseButtonDown = false;
        }
        if (event.MouseInput.Event == EMIE_MOUSE_MOVED) {
            mousePosition = irr::core::position2di{
                    event.MouseInput.Y,
                    event.MouseInput.X}; // I have no idea why the order is reversed
        }
    }
    for (const auto& handler : eventHandlers) {
        handler.second(event);
    }
    return false;
}

irr::core::vector2di IrrEventReceiver::getMouseDelta()
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    // std::lock_guard<std::recursive_mutex> lock2(irrlichtMutex);
    if (auto newViewport = graphicsGetViewport(); viewport != newViewport) {
        viewport = newViewport;
        mouseUpdateFlag = true;
    }
    if (mouseUpdateFlag) {
        mouseUpdateFlag = false;
        return {0, 0};
    }
    auto ret = mousePosition - lastSeenMousePosition;
    lastSeenMousePosition = mousePosition;
    auto center = graphicsGetViewport().getCenter();
    // auto ret = mousePosition - center;

    {
        LOG("mouse :: Acquiring irrlicht lock...");
        std::lock_guard<std::recursive_mutex> irrlock(irrlichtMutex);
        LOG("mouse :: Acquired irrlicht lock");
        auto cursorControl = graphics::irrDevice->getCursorControl();
        if ((cursorControl->getPosition() - center).getLengthSQ() < 100 * 100) {
            // assert("duck" == "not duck");
            // cursorControl->setPosition(center);
            // lastSeenMousePosition = center;
        }
        cursorControl->setVisible(false);
        LOG("mouse :: Released irrlicht lock");
    }

    return ret;
}

// Определяет, нажата ли заданная клавиша на клавиатуре
bool IrrEventReceiver::isKeyPressed(irr::EKEY_CODE key) const
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return pressedKeys.count(key) > 0;
}

// Добавляет пользоваетельский обработчик событий
uint64_t IrrEventReceiver::addEventHandler(const EventHandlerType& handler)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return eventHandlers.insert(handler);
}

// Удаляет пользоваетельский обработчик событий
void IrrEventReceiver::deleteEventHandler(uint64_t id)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    eventHandlers.remove(id);
}

// Включает/выключает видимость прицела
void setAimVisible(bool visible)
{
    graphics::aimVisible = visible;
}

// Инициализация графического движко
static void initializeIrrlicht(std::vector<std::string>& args);

// Внешнее API: создание куба как объекта
FuncResult handlerGraphicsCreateCube(UNUSED const std::vector<std::string>& args)
{
    FuncResult ret;
    ret.data.resize(1);

    std::lock_guard<std::recursive_mutex> lock(gameObjectMutex);

    GameObject* obj = new GameObject(graphicsCreateCube());
    auto objectHandle = registerGameObject(obj);

    setReturn<uint64_t>(ret, 0, objectHandle);
    return ret;
}

// Внешнее API: перемещение оъекта
FuncResult handlerGraphicsMoveObject(const std::vector<std::string>& args)
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

// Внешнее API: удаление оъекта
FuncResult handlerGraphicsDeleteObject(const std::vector<std::string>& args)
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

// Внешнее API: вращение оъекта
FuncResult handlerGraphicsRotateObject(const std::vector<std::string>& args)
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

// Внешнее API: загрузить текстуру из файла
FuncResult handlerGraphicsLoadTexture(const std::vector<std::string>& args)
{
    std::lock_guard<std::recursive_mutex> irrlock(irrlichtMutex);
    if (args.size() != 1) {
        throw std::logic_error("Wrong number of arguments for handlerGraphicsLoadTexture()");
    }
    FuncResult ret;

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

// Внешнее API: добавить текстуру к объекту
FuncResult handlerGraphicsAddTexture(const std::vector<std::string>& args)
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

// Внешнее API: добавить текстуру к модели
FuncResult handlerGraphicsDrawableAddTexture(const std::vector<std::string>& args)
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

// Создать куб как модель
scene::ISceneNode* graphicsCreateDrawableCube();

// Внешнее API: создать куб как модель
FuncResult handlerCreateDrawableCube(const std::vector<std::string>& args)
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

// Внешнее API: включить физику для объекта
FuncResult handlerDrawableEnablePhysics(const std::vector<std::string>& args)
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

FuncResult handlerAdd2DRectangle(const std::vector<std::string>& args)
{
    if (args.size() != 8) {
        throw std::logic_error("Wrong number of arguments for handlerAdd2DRectangle()");
    }
    FuncResult ret;
    ret.data.resize(1);

    LOG("Args:");
    for (const auto& i : args) {
        LOG("  " << i);
    }

    auto x1 = getArgument<float>(args, 0);
    auto y1 = getArgument<float>(args, 1);
    auto x2 = getArgument<float>(args, 2);
    auto y2 = getArgument<float>(args, 3);
    auto r = getArgument<uint>(args, 4);
    auto g = getArgument<uint>(args, 5);
    auto b = getArgument<uint>(args, 6);
    auto a = getArgument<uint>(args, 7);

    uint64_t handle = graphicsAdd2DRectangle(/* rect */ {x1, y1, x2, y2},
                                             /* color */ {a, r, g, b});
    setReturn(ret, 0, handle);
    return ret;
}
FuncResult handlerAdd2DLine(const std::vector<std::string>& args)
{
    if (args.size() != 8) {
        throw std::logic_error("Wrong number of arguments for handlerAdd2DLine()");
    }
    FuncResult ret;
    ret.data.resize(1);

    auto x1 = getArgument<float>(args, 0);
    auto y1 = getArgument<float>(args, 1);
    auto x2 = getArgument<float>(args, 2);
    auto y2 = getArgument<float>(args, 3);
    auto r = getArgument<uint>(args, 4);
    auto g = getArgument<uint>(args, 5);
    auto b = getArgument<uint>(args, 6);
    auto a = getArgument<uint>(args, 7);

    uint64_t handle = graphicsAdd2DLine(/* line */ {x1, y1, x2, y2},
                                        /* color */ {a, r, g, b});
    setReturn(ret, 0, handle);
    return ret;
}
FuncResult handlerAdd2DImage(const std::vector<std::string>& args)
{
    if (args.size() != 5) {
        throw std::logic_error("Wrong number of arguments for handlerAdd2DImage()");
    }
    FuncResult ret;
    ret.data.resize(1);

    auto x1 = getArgument<float>(args, 0);
    auto y1 = getArgument<float>(args, 1);
    auto x2 = getArgument<float>(args, 2);
    auto y2 = getArgument<float>(args, 3);
    auto imageHandle = getArgument<uint64_t>(args, 4);
    auto texture = accessTexture(imageHandle);

    uint64_t handle = graphicsAdd2DImage(/* rect */ {x1, y1, x2, y2}, texture);
    setReturn(ret, 0, handle);
    return ret;
}

FuncResult handlerRemove2DRectangle(const std::vector<std::string>& args)
{
    if (args.size() != 1) {
        throw std::logic_error("Wrong number of arguments for handlerRemove2DRectangle()");
    }
    FuncResult ret;

    auto handle = getArgument<uint64_t>(args, 0);
    graphicsRemove2DRectangle(handle);
    return ret;
}
FuncResult handlerRemove2DLine(const std::vector<std::string>& args)
{
    if (args.size() != 1) {
        throw std::logic_error("Wrong number of arguments for handlerRemove2DLine()");
    }
    FuncResult ret;

    auto handle = getArgument<uint64_t>(args, 0);
    graphicsRemove2DLine(handle);
    return ret;
}
FuncResult handlerRemove2DImage(const std::vector<std::string>& args)
{
    if (args.size() != 1) {
        throw std::logic_error("Wrong number of arguments for handlerRemove2DImage()");
    }
    FuncResult ret;

    auto handle = getArgument<uint64_t>(args, 0);
    graphicsRemove2DImage(handle);
    return ret;
}

FuncResult handlerModify2DRectangle(const std::vector<std::string>& args)
{
    if (args.size() != 9) {
        throw std::logic_error("Wrong number of arguments for handlerModify2DRectangle()");
    }
    FuncResult ret;

    auto handle = getArgument<uint64_t>(args, 0);
    auto x1 = getArgument<float>(args, 1);
    auto y1 = getArgument<float>(args, 2);
    auto x2 = getArgument<float>(args, 3);
    auto y2 = getArgument<float>(args, 4);
    auto r = getArgument<uint>(args, 5);
    auto g = getArgument<uint>(args, 6);
    auto b = getArgument<uint>(args, 7);
    auto a = getArgument<uint>(args, 8);

    graphicsModify2DRectangle(handle, /* rect */ {x1, y1, x2, y2}, /* color */ {a, r, g, b});
    return ret;
}
FuncResult handlerModify2DLine(const std::vector<std::string>& args)
{
    if (args.size() != 9) {
        throw std::logic_error("Wrong number of arguments for handlerModify2DLine()");
    }
    FuncResult ret;

    auto handle = getArgument<uint64_t>(args, 0);
    auto x1 = getArgument<float>(args, 1);
    auto y1 = getArgument<float>(args, 2);
    auto x2 = getArgument<float>(args, 3);
    auto y2 = getArgument<float>(args, 4);
    auto r = getArgument<uint>(args, 5);
    auto g = getArgument<uint>(args, 6);
    auto b = getArgument<uint>(args, 7);
    auto a = getArgument<uint>(args, 8);

    graphicsModify2DLine(handle, /* line */ {x1, y1, x2, y2}, /* color */ {a, r, g, b});
    return ret;
}
FuncResult handlerModify2DImage(const std::vector<std::string>& args)
{
    if (args.size() != 6) {
        throw std::logic_error("Wrong number of arguments for handlerModify2DImage()");
    }
    FuncResult ret;

    auto handle = getArgument<uint64_t>(args, 0);
    auto x1 = getArgument<float>(args, 1);
    auto y1 = getArgument<float>(args, 2);
    auto x2 = getArgument<float>(args, 3);
    auto y2 = getArgument<float>(args, 4);
    auto imageHandle = getArgument<uint64_t>(args, 5);
    auto texture = accessTexture(imageHandle);

    graphicsModify2DImage(handle, /* rect */ {x1, y1, x2, y2}, texture);
    return ret;
}

// Инициализация внешнего API
static inline void initializeGraphicsFuncProviders()
{
    registerFuncProvider(FuncProvider("graphics.createCube", handlerGraphicsCreateCube), "", "u");
    registerFuncProvider(
            FuncProvider("graphics.moveObject", handlerGraphicsMoveObject), "ufff", "");
    registerFuncProvider(
            FuncProvider("graphics.rotateObject", handlerGraphicsRotateObject), "ufff", "");
    registerFuncProvider(
            FuncProvider("graphics.deleteObject", handlerGraphicsDeleteObject), "u", "");
    registerFuncProvider(
            FuncProvider("graphics.texture.loadFromFile", handlerGraphicsLoadTexture), "s", "u");
    registerFuncProvider(FuncProvider("graphics.texture.add", handlerGraphicsAddTexture), "uu", "");
    registerFuncProvider(
            FuncProvider("graphics.texture.addToDrawable", handlerGraphicsDrawableAddTexture),
            "uu",
            "");
    registerFuncProvider(
            FuncProvider("graphics.drawable.createCube", handlerCreateDrawableCube), "", "u");
    registerFuncProvider(
            FuncProvider("graphics.drawable.enablePhysics", handlerDrawableEnablePhysics),
            "ufff",
            "");
    registerFuncProvider(
            FuncProvider("graphics.2d.addRectangle", handlerAdd2DRectangle), "ffffiiii", "u");
    registerFuncProvider(FuncProvider("graphics.2d.addLine", handlerAdd2DLine), "ffffiiii", "u");
    registerFuncProvider(FuncProvider("graphics.2d.addImage", handlerAdd2DImage), "ffffu", "u");
    registerFuncProvider(FuncProvider("graphics.2d.modifyRectangle", handlerModify2DRectangle),
                         "uffffiiii",
                         "u");
    registerFuncProvider(
            FuncProvider("graphics.2d.modifyLine", handlerModify2DLine), "uffffiiii", "u");
    registerFuncProvider(
            FuncProvider("graphics.2d.modifyImage", handlerModify2DImage), "uffffu", "u");
    registerFuncProvider(
            FuncProvider("graphics.2d.removeRectangle", handlerRemove2DRectangle), "u", "");
    registerFuncProvider(FuncProvider("graphics.2d.removeLine", handlerRemove2DLine), "u", "");
    registerFuncProvider(FuncProvider("graphics.2d.removeImage", handlerRemove2DImage), "u", "");
}

// Освобождение ресурсов
void cleanupGraphics()
{
    graphics::terrainSelector->drop();
    graphics::irrDevice->drop();
}

// Инициализация графической системы
void initializeGraphics(std::vector<std::string>& args)
{
    initializeIrrlicht(args);
    initializeGraphicsFuncProviders();
}

// Инициаллизация Irrlicht
static void initializeIrrlicht(UNUSED std::vector<std::string>& args)
{
    graphics::irrDevice = irr::createDevice(
            irr::video::EDT_OPENGL, // Драйвер для рендеринга (здесь OpenGL, но пока
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

// Создать куб как объект
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

// Отрисовать кадр
void graphicsDraw()
{
    // Внимание: эту функцию можно вызывать только из основного потока.
    // Если кто-то вызовет её из другого потока, случится страшное
    // (вроде Segfault в glGenTextures, только ещё страшнее)
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    graphics::irrVideoDriver->beginScene(
            true, // Неясно, что это
            true, // Неясно, что это
            irr::video::SColor(255,
                               100,
                               101,
                               140)); // Какой-то цвет, возможно, цвет фона (ARGB)

    //    graphics::irrSceneManager->addCameraSceneNode(0,
    //    irr::core::vector3df(0,30,-40), irr::core::vector3df(0,5,0));

    graphics::irrGuiEnvironment->drawAll();
    graphics::irrSceneManager->drawAll();
    if (graphics::aimVisible) {
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
    }
    for (auto& [_, rc] : graphics::rectangles) {
        auto& [rect, color] = rc;
        graphics::irrVideoDriver->draw2DRectangle(color, graphicsViewportize(rect));
    }
    for (auto& [_, lc] : graphics::lines) {
        auto& [line, color] = lc;
        auto vpline = graphicsViewportize(line);
        graphics::irrVideoDriver->draw2DLine(vpline.start, vpline.end, color);
    }
    for (auto& [_, pi] : graphics::images) {
        auto& [rect, image] = pi;
        graphics::irrVideoDriver->draw2DImage(
                image, graphicsViewportize(rect), irr::core::recti{{0, 0}, image->getSize()});
    }
    graphics::irrVideoDriver->endScene();
}

// Перевести координаты из [0.0 .. 1.0] в [0; width] || [top; height]
irr::core::recti graphicsViewportize(const irr::core::rectf& rect)
{
    auto viewport = graphics::irrVideoDriver->getViewPort();
    auto scaleX = viewport.getWidth();
    auto scaleY = viewport.getHeight();
    auto upperLeft = rect.UpperLeftCorner;
    auto lowerRight = rect.LowerRightCorner;
    auto left = upperLeft.X;
    auto top = upperLeft.Y;
    auto right = lowerRight.X;
    auto bottom = lowerRight.Y;
    return irr::core::recti({static_cast<int>(left * scaleX),
                             static_cast<int>(top * scaleY),
                             static_cast<int>(right * scaleX),
                             static_cast<int>(bottom * scaleY)});
}
irr::core::line2di graphicsViewportize(const irr::core::line2df& line)
{
    auto viewport = graphics::irrVideoDriver->getViewPort();
    auto scaleX = viewport.getWidth();
    auto scaleY = viewport.getHeight();
    auto x1 = line.start.X;
    auto y1 = line.start.Y;
    auto x2 = line.end.X;
    auto y2 = line.end.Y;
    return irr::core::line2di({static_cast<int>(x1 * scaleX),
                               static_cast<int>(y1 * scaleY),
                               static_cast<int>(x2 * scaleX),
                               static_cast<int>(y2 * scaleY)});
}
irr::core::position2di graphicsViewportize(const irr::core::position2df& pos)
{
    auto viewport = graphics::irrVideoDriver->getViewPort();
    auto scaleX = viewport.getWidth();
    auto scaleY = viewport.getHeight();
    auto x = pos.X;
    auto y = pos.Y;
    return irr::core::position2di{static_cast<int>(x * scaleX), static_cast<int>(y * scaleY)};
}

// Переместить объект
void graphicsMoveObject(ISceneNode* obj, double x, double y, double z)
{
    return addDrawFunction([=]() {
        if (obj == nullptr) {
            return;
        }
        std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);

        obj->setPosition(core::vector3df(x, y, z));
    });
}
// Переместить объект
void graphicsMoveObject(ISceneNode* obj, const core::vector3df& pos)
{
    return addDrawFunction([=]() {
        if (obj == nullptr) {
            return;
        }
        std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
        obj->setPosition(pos);
    });
}
// Переместить объект
void graphicsMoveObject(ISceneNode* obj, const GamePosition& gp)
{
    if (obj == nullptr) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    obj->setPosition(gp.toIrrVector3df());
}

// Удалить объект
void graphicsDeleteObject(GameObject* obj)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    obj->sceneNode()->remove();
    delete obj;
}

// Повернуть объект
void graphicsRotateObject(ISceneNode* obj, const core::vector3df& rot)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    if (obj == nullptr) {
        return;
    }
    obj->setRotation(rot);
}

// Загрузить текстуру из файла
ITexture* graphicsLoadTexture(const std::string& textureFileName)
{
    if (auto it = graphics::textureCache.find(textureFileName);
        it != graphics::textureCache.end()) {
        LOG("loading cached texture: " << textureFileName);
        return it->second;
    }
    return addDrawFunction([=]() -> ITexture* {
        std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
        LOG("loading texture: " << textureFileName);
        ITexture* texture = graphics::irrVideoDriver->getTexture(textureFileName.c_str());
        if (texture == nullptr) {
            LOG("Loading texture failed");
            throw std::runtime_error("Loading texture failed");
        }
        LOG("Texture loaded successfully");
        texture->grab();
        graphics::textureCache.emplace(textureFileName, texture);
        return texture;
    });
}

// Добавить текстуру к объекту
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

// Загрузить чанк в память
// Код частично взят с http://irrlicht.sourceforge.net/docu/example012.html
void graphicsLoadTerrain(int64_t off_x,
                         int64_t off_y,
                         const std::string& heightmap,
                         video::ITexture* tex,
                         video::ITexture* detail)
{
    double irrOffsetX = CHUNK_SIZE_IRRLICHT * off_x;

    double irrOffsetY = CHUNK_SIZE_IRRLICHT * off_y;
    scene::ITerrainSceneNode* terrain;

    {
        std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
        terrain = graphics::irrSceneManager->addTerrainSceneNode(
                heightmap.c_str(),                                          // heightmap filename
                nullptr,                                                    // parent node
                -1,                                                         // node id
                core::vector3df(irrOffsetX - 180, -1250, irrOffsetY - 200), // position
                core::vector3df(0.0f, 0.0f, 0.0f),                          // rotation
                core::vector3df(10.0f, 4.0f, 10.0f),                        // scale
                video::SColor(255, 255, 255, 255),                          // vertexColor (?)
                5,              // maxLOD (Level Of Detail)
                scene::ETPS_17, // patchSize (?)
                4               // smoothFactor (?)
        );
        if (terrain == nullptr) {
            throw std::runtime_error("unable to create terrain scene node");
        }
        terrain->setMaterialFlag(irr::video::EMF_LIGHTING, false);
        terrain->setMaterialTexture(1, tex);
        terrain->setMaterialTexture(0, detail);
        terrain->scaleTexture(1.0f, 20.0f);
    }
    Chunk terrainChunk({}, terrain);
    terrainManager.addChunk(off_x, off_y, std::move(terrainChunk));
}

// Загрузить чанк в память, использовать irr::video::IImage* в качестве массива высот
void graphicsLoadTerrain(int64_t off_x,
                         int64_t off_y,
                         video::IImage* heightmap,
                         video::ITexture* tex,
                         video::ITexture* detail)
{
    terrainManager.writeTerrain(off_x, off_y, heightmap);
    graphicsLoadTerrain(off_x, off_y, terrainManager.getTerrainFilename(off_x, off_y), tex, detail);
}

static std::unordered_map<scene::ISceneNode*, scene::ITriangleSelector*> triangleSelectors;

// Включить взаимодействие других объектов и игрока с данным объектом
void graphicsHandleCollisions(scene::ITerrainSceneNode* node)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto selector = graphics::irrSceneManager->createTerrainTriangleSelector(node);
    if (selector == nullptr) {
        throw std::runtime_error("unable to create triangle selector on terrain scene node");
    }

    triangleSelectors.insert({node, selector});

    static_cast<scene::IMetaTriangleSelector*>(
            static_cast<scene::ISceneNodeAnimatorCollisionResponse*>(
                    *graphics::pseudoCamera->getAnimators().begin())
                    ->getWorld())
            ->addTriangleSelector(selector);
    selector->drop();
}

// Выключить взаимодействие других объектов и игрока с данным объектом
void graphicsStopHandlingCollisions(scene::ITerrainSceneNode* node)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);

    auto metaSelector = static_cast<scene::IMetaTriangleSelector*>(
            static_cast<scene::ISceneNodeAnimatorCollisionResponse*>(
                    *graphics::pseudoCamera->getAnimators().begin())
                    ->getWorld());
    metaSelector->removeTriangleSelector(triangleSelectors.at(node));
}

// Включить взаимодействие других объектов и игрока с данным набором вершин
void graphicsHandleCollisionsMesh(scene::IMesh* mesh, scene::ISceneNode* node)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto selector = graphics::irrSceneManager->createTriangleSelector(mesh, node);
    if (selector == nullptr) {
        throw std::runtime_error("unable to create triangle selector on mesh scene node");
    }
    triangleSelectors.insert({node, selector});

    static_cast<scene::IMetaTriangleSelector*>(
            static_cast<scene::ISceneNodeAnimatorCollisionResponse*>(
                    *graphics::pseudoCamera->getAnimators().begin())
                    ->getWorld())
            ->addTriangleSelector(selector);
    selector->drop();
}

// Включить взаимодействие других объектов и игрока с данным irr::scene::ISceneNode* по
// его bounding box
void graphicsHandleCollisionsBoundingBox(scene::ISceneNode* node)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto selector = graphics::irrSceneManager->createTriangleSelectorFromBoundingBox(node);
    if (selector == nullptr) {
        throw std::runtime_error("unable to create triangle selector on scene node bounding box");
    }
    triangleSelectors.insert({node, selector});

    static_cast<scene::IMetaTriangleSelector*>(
            static_cast<scene::ISceneNodeAnimatorCollisionResponse*>(
                    *graphics::pseudoCamera->getAnimators().begin())
                    ->getWorld())
            ->addTriangleSelector(selector);
    selector->drop();
}

// Включить физику для заданного irr::scene::ISceneNode*
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

// Выключить физику для заданного irr::scene::ISceneNode*
void graphicsDisablePhysics(scene::ISceneNode* node)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    node->removeAnimators();
}

// Инициализации физики и обсчёта коллизий
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

// Получить объект камеры
irr::scene::ICameraSceneNode* graphicsGetCamera()
{
    return graphics::camera;
}

// Получить объект псевдокамеры, которая используется для обсчёта физики
irr::scene::ISceneNode* graphicsGetPseudoCamera()
{
    return graphics::pseudoCamera;
}

// Вызывает метод IrrlictDevice::run() и возвращает его результат
bool irrDeviceRun()
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    return graphics::irrDevice->run();
}

// Возвращает обработчик событий графического движка
[[deprecated]] const IrrEventReceiver& getKeyboardEventReceiver()
{
    return graphics::irrEventReceiver;
}

// Определяет, куда поставить объект, если даны положение камеры и точкпа, куда камера
// направлена
std::pair<bool, GamePosition> graphicsGetPlacePosition(const GamePosition& pos,
                                                       const GamePosition& target)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
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

// Создаёт ISceneNode* по набору вершин
scene::ISceneNode* graphicsCreateMeshSceneNode(scene::IMesh* mesh)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto node = graphics::irrSceneManager->addMeshSceneNode(mesh);
    if (node == nullptr) {
        throw std::runtime_error("unable to create mesh scene node");
    }
    return node;
}

// Загружает набор вершин из файла
scene::IMesh* graphicsLoadMesh(const std::string& filename)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto mesh = graphics::irrSceneManager->getMesh(filename.c_str());
    if (mesh == nullptr) {
        throw std::runtime_error("unable to load mesh from file");
    }
    return mesh;
}

// Создаёт куб как модель
scene::ISceneNode* graphicsCreateDrawableCube()
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto node = graphics::irrSceneManager->addCubeSceneNode();
    if (node == nullptr) {
        throw std::runtime_error("unable to add cube scene node");
    }
    return node;
}

// Вызывает функцию прыжка у объекта со включённой физикой
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

// Вызывает функцию движения вперёд у объекта
void graphicsStep(scene::ISceneNode* node, float distance)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto direction = node->getRotation().rotationToDirection().normalize();
    node->setPosition(node->getPosition() + direction * distance);
}

// Поворачивает ISceneNode* к даннай точке
void graphicsLookAt(scene::ISceneNode* node, float x, float y, float z)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    core::vector3df src = node->getAbsolutePosition();
    core::vector3df dst(x, y, z);
    core::vector3df diff = dst - src;
    node->setRotation(diff.getHorizontalAngle());
}

// Получает положение ISceneNode* и возвращает его через 3 ссылки
void graphicsGetPosition(scene::ISceneNode* node, float& x, float& y, float& z)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    std::array<float, 3> arr;
    node->getPosition().getAs3Values(arr.data());
    x = arr[0];
    y = arr[1];
    z = arr[2];
}

// Создаёт listbox в виде объекта графического движка
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

// Возвращает обработчик событий
IrrEventReceiver& getEventReceiver()
{
    return graphics::irrEventReceiver;
}

// Применяет функтор на прямоугольной области ландшафта чанка
void graphicsModifyTerrain(ITerrainSceneNode* terrain,
                           int x1,
                           int y1,
                           int x2,
                           int y2,
                           const std::function<int(int, int, int)>& func)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto mesh = terrain->getMesh();
    for (uint i = 0; i < mesh->getMeshBufferCount(); ++i) {
        auto meshbuf = mesh->getMeshBuffer(i);
        if (meshbuf->getVertexType() != irr::video::EVT_2TCOORDS) {
            continue;
        }
        auto vertices = static_cast<irr::video::S3DVertex2TCoords*>(meshbuf->getVertices());

        for (int x = x1; x < x2; ++x) {
            for (int y = y1; y < y2; ++y) {
                int index = y * CHUNK_SIZE + x;
                assert(index >= 0 && index < CHUNK_SIZE * CHUNK_SIZE);
                vertices[index].Pos.Y = std::clamp(func(x, y, vertices[index].Pos.Y), 0, 255);
            }
        }

        meshbuf->setDirty();
        meshbuf->recalculateBoundingBox();
    }
    terrain->setPosition(terrain->getPosition()); // Does not work without it

    // Update collision information
    graphicsStopHandlingCollisions(terrain);
    graphicsHandleCollisions(terrain);
}

// Применяет функтор на прямоугольной области ландшафта чанка, не изменяя этот ландшафт
void graphicsVisitTerrain(ITerrainSceneNode* terrain,
                          int x1,
                          int y1,
                          int x2,
                          int y2,
                          const std::function<void(int, int, int)>& func)
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    auto mesh = terrain->getMesh();
    for (uint i = 0; i < mesh->getMeshBufferCount(); ++i) {
        auto meshbuf = mesh->getMeshBuffer(i);
        if (meshbuf->getVertexType() != irr::video::EVT_2TCOORDS) {
            continue;
        }
        auto vertices = static_cast<irr::video::S3DVertex2TCoords*>(meshbuf->getVertices());

        for (int x = x1; x < x2; ++x) {
            for (int y = y1; y < y2; ++y) {
                int index = y * CHUNK_SIZE + x;
                assert(index >= 0 && index < CHUNK_SIZE * CHUNK_SIZE);
                func(x, y, vertices[index].Pos.Y);
            }
        }
    }
}

// Возвращает объект IVideoDriver*
irr::video::IVideoDriver* getIrrlichtVideoDriver()
{
    std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
    return graphics::irrVideoDriver;
}

uint64_t graphicsAdd2DRectangle(const irr::core::rectf& rect, const irr::video::SColor& color)
{
    return graphics::rectangles.insert({rect, color});
}

uint64_t graphicsAdd2DLine(const irr::core::line2df& line, const irr::video::SColor& color)
{
    return graphics::lines.insert({line, color});
}
uint64_t graphicsAdd2DImage(const irr::core::rectf& rect, irr::video::ITexture* texture)
{
    return graphics::images.insert({rect, texture});
}

void graphicsRemove2DRectangle(uint64_t handle)
{
    graphics::rectangles.remove(handle);
}
void graphicsRemove2DLine(uint64_t handle)
{
    graphics::lines.remove(handle);
}
void graphicsRemove2DImage(uint64_t handle)
{
    graphics::images.access(handle).second->drop();
    graphics::images.remove(handle);
}

void graphicsModify2DRectangle(uint64_t handle,
                               const irr::core::rectf& rect,
                               const irr::video::SColor& color)
{
    graphics::rectangles.mutableAccess(handle) = {rect, color};
}
void graphicsModify2DLine(uint64_t handle,
                          const irr::core::line2df& line,
                          const irr::video::SColor& color)
{
    graphics::lines.mutableAccess(handle) = {line, color};
}
void graphicsModify2DImage(uint64_t handle,
                           const irr::core::rectf& rect,
                           irr::video::ITexture* texture)
{
    graphics::images.access(handle).second->drop();
    graphics::images.mutableAccess(handle) = {rect, texture};
}

irr::core::recti graphicsGetViewport()
{
    return graphics::irrVideoDriver->getViewPort();
}
