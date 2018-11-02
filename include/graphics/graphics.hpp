#ifndef GRAPHICS_GRAPHICS_HPP
#define GRAPHICS_GRAPHICS_HPP

#include <string>
#include <unordered_set>
#include <vector>

#include <modbox/geometry/game_position.hpp>
#include <modbox/util/handle_storage.hpp>
#include <modbox/game/game_object.hpp>

#include <boost/algorithm/string.hpp>
#include <irrlicht_wrapper.hpp>

using namespace irr;
using namespace scene;
using namespace video;

void drawBarrier();

// ===== Initialization and cleanup =====

/// Initialize graphical engine and related data structures
/// Should be called once when the game is started
void initializeGraphics(std::vector<std::string>&);

/// Stop graphical engine and free related resources
void cleanupGraphics();

// ===== Managing objects =====

/// Move a scene node to the specified location
void graphicsMoveObject(ISceneNode*, double, double, double);
void graphicsMoveObject(ISceneNode*, const core::vector3df&);
void graphicsMoveObject(ISceneNode*, const GamePosition&);

/// Rotate a scene node
void graphicsRotateObject(ISceneNode* obj, const core::vector3df& rot);

std::pair<bool, GamePosition> graphicsGetPlacePosition(const GamePosition& pos,
                                                       const GamePosition& target);

void graphicsEnablePhysics(scene::ISceneNode* node,
                           const core::vector3df& radius = core::vector3df(10, 10, 10));
void graphicsDisablePhysics(scene::ISceneNode* node);

void graphicsJump(scene::ISceneNode* node, float jumpSpeed);
void graphicsStep(scene::ISceneNode* node, float distance);
void graphicsLookAt(scene::ISceneNode* node, float x, float y, float z);
void graphicsGetPosition(scene::ISceneNode* node, float& x, float& y, float& z);

uint64_t graphicsAdd2DRectangle(const irr::core::rectf& rect, const irr::video::SColor& color);
uint64_t graphicsAdd2DLine(const irr::core::line2df& line, const irr::video::SColor& color);
uint64_t graphicsAdd2DImage(const irr::core::rectf& rect, irr::video::ITexture* texture);

void graphicsModify2DRectangle(uint64_t handle,
                               const irr::core::rectf& rect,
                               const irr::video::SColor& color);
void graphicsModify2DLine(uint64_t handle,
                          const irr::core::line2df& line,
                          const irr::video::SColor& color);
void graphicsModify2DImage(uint64_t handle,
                           const irr::core::rectf& rect,
                           irr::video::ITexture* texture);

void graphicsRemove2DRectangle(uint64_t handle);
void graphicsRemove2DLine(uint64_t handle);
void graphicsRemove2DImage(uint64_t handle);

// ===== Managing textures =====

/// Load texture and return Irrlicht ITexture* instance for this texture
ITexture* graphicsLoadTexture(const std::string&);

/// Add texture to a game object
void graphicsAddTexture(const GameObject&, ITexture*);

// ===== Drawing =====

/// Draw all necessary objects
void graphicsDraw();

// ===== Managing camera =====

/// Return active ICameraSceneNode pointer
irr::scene::ICameraSceneNode* graphicsGetCamera();
irr::scene::ISceneNode* graphicsGetPseudoCamera();

// ===== Managing terrain =====

/// Create a terrain scene node
// TODO
void graphicsLoadTerrain(int64_t off_x,
                         int64_t off_y,
                         const std::string& heightmap,
                         video::ITexture* tex,
                         video::ITexture* detail);

void graphicsLoadTerrain(int64_t off_x,
                         int64_t off_y,
                         video::IImage* heightmap,
                         video::ITexture* tex,
                         video::ITexture* detail);

void graphicsInitializeCollisions();
void graphicsHandleCollisions(scene::ITerrainSceneNode* node);
void graphicsHandleCollisionsMesh(scene::IMesh* mesh, scene::ISceneNode* node);
void graphicsHandleCollisionsBoundingBox(scene::ISceneNode* node);
void graphicsStopHandlingCollisions(irr::scene::ISceneNode* node);

// ===== Utility functions =====

/// Poll for Ittlicht events related to window and similar stuff
/// Return false, if the window should be closed and false otherwise
bool irrDeviceRun();

// Irrlicht has terrible method and object member naming style
// But I actually have no choice... But wait, I have an idea
#define onEvent OnEvent
#define keyInput KeyInput
#define key Key
#define pressedDown PressedDown
#define eventType EventType
// Yeah, now it's much better
/// Event receiver for keyboard events
class IrrEventReceiver : public irr::IEventReceiver
{
public:
    using EventHandlerType = std::function<bool(const irr::SEvent&)>;
    IrrEventReceiver();
    virtual ~IrrEventReceiver() = default;

    virtual bool onEvent(const irr::SEvent& event) override;
    virtual bool isKeyPressed(irr::EKEY_CODE key) const;

    uint64_t addEventHandler(const EventHandlerType& handler);
    void deleteEventHandler(uint64_t id);

    irr::core::vector2di getMouseDelta();

private:
    bool mouseUpdateFlag = true;
    bool leftMouseButtonDown = false;
    bool middleMouseButtonDown = false;
    bool rightMouseButtonDown = false;

    irr::gui::ICursorControl* cursorControl;
    irr::core::position2di mousePosition;
    irr::core::position2di lastSeenMousePosition;
    irr::core::recti viewport;
    std::unordered_set<irr::EKEY_CODE> pressedKeys;
    HandleStorage<uint64_t, EventHandlerType> eventHandlers;
    mutable std::recursive_mutex mutex;
};
// And... Clean up
#undef onEvent
#undef keyInput
#undef key
#undef pressedDown
#undef eventType

IrrEventReceiver& getEventReceiver();

scene::ISceneNode* graphicsCreateMeshSceneNode(scene::IMesh* mesh);
scene::IMesh* graphicsLoadMesh(const std::string& filename);

class DrawablesManager
{
public:
    DrawablesManager() = default;
    DrawablesManager(const DrawablesManager& other) = delete;
    DrawablesManager(DrawablesManager&& other) = default;
    virtual ~DrawablesManager() = default;

    DrawablesManager& operator=(const DrawablesManager& other) = delete;
    DrawablesManager& operator=(DrawablesManager&& other) = default;

    irr::scene::ISceneNode* access(uint64_t handle);
    uint64_t track(irr::scene::ISceneNode* drawable);
    std::optional<uint64_t> reverseLookup(irr::scene::ISceneNode* drawable);
    void forget(uint64_t handle);

private:
    HandleStorage<uint64_t, irr::scene::ISceneNode*> drawables;
};

extern DrawablesManager drawablesManager;

irr::gui::IGUIListBox* createListBox(const std::vector<std::wstring>& strings,
                                     const core::recti& position);

void graphicsInitializeCollisions();

uint64_t addIrrlichtEventHandler(const std::function<bool(const irr::SEvent&)>& callback);

void setAimVisible(bool visible);
void graphicsModifyTerrain(ITerrainSceneNode* terrain,
                           int x1,
                           int y1,
                           int x2,
                           int y2,
                           const std::function<int(int, int, int)>& func);
void graphicsVisitTerrain(ITerrainSceneNode* terrain,
                          int x1,
                          int y1,
                          int x2,
                          int y2,
                          const std::function<void(int, int, int)>& func);
irr::video::IVideoDriver* getIrrlichtVideoDriver();

irr::core::recti graphicsViewportize(const irr::core::rectf& rect);
irr::core::line2di graphicsViewportize(const irr::core::line2df& rect);
irr::core::position2di graphicsViewportize(const irr::core::position2df& rect);

irr::core::recti graphicsGetViewport();

void addSelectorKind(const std::string& kind);
void removeSelectorKind(const std::string& kind);
void addSubSelector(const std::string& kind, irr::scene::ITriangleSelector* selector);
void removeSubSelector(const std::string& kind, irr::scene::ITriangleSelector* selector);
std::optional<std::pair<irr::core::vector3df, irr::scene::ISceneNode*>> getRayIntersect(const irr::core::vector3df& origin, const irr::core::vector3df& end, const std::string& kind);

irr::core::vector3df getCameraTarget(float len);

irr::scene::ITriangleSelector* graphicsCreateTriangleSelector(irr::scene::ISceneNode* node);

#endif /* end of include guard: GRAPHICS_GRAPHICS_HPP */
