#ifndef GRAPHICS_GRAPHICS_HPP
#define GRAPHICS_GRAPHICS_HPP

#include <string>
#include <unordered_set>
#include <vector>

#include <modbox/game/objects/objects.hpp>
#include <modbox/geometry/game_position.hpp>
#include <modbox/util/handle_storage.hpp>

#include <boost/algorithm/string.hpp>
#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace video;

void drawBarrier();

// ===== Initialization and cleanup =====

/// Initialize graphical engine and related data structures
/// Should be called once when the game is started
void initializeGraphics(std::vector<std::string>&);

/// Stop graphical engine and free related resources
/// Should be called once when the game is exited
void cleanupGraphics();

// ===== Managing objects =====

/// Create a cube and return the GameObject instance
GameObjCube graphicsCreateCube();

/// Move a scene node to the specified location
void graphicsMoveObject(ISceneNode*, double, double, double);
void graphicsMoveObject(ISceneNode*, const core::vector3df&);
void graphicsMoveObject(ISceneNode*, const GamePosition&);

/// Rotate a scene node
void graphicsRotateObject(ISceneNode* obj, const core::vector3df& rot);

/// Delete and unregister a game object
void graphicsDeleteObject(GameObject*);

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
    IrrEventReceiver() = default;
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
    void forget(uint64_t handle);

private:
    HandleStorage<uint64_t, irr::scene::ISceneNode*> drawables;
};

extern DrawablesManager drawablesManager;

irr::gui::IGUIListBox* createListBox(const std::vector<std::wstring>& strings,
                                     const core::recti& position);

void graphicsInitializeCollisions();

uint64_t addEventHandler(const std::function<bool(const irr::SEvent&)>& callback);

// Это ужасный код, но он поможет при отладке
static inline std::string getGuiEventDebugName(irr::gui::EGUI_EVENT_TYPE type)
{
#define ENTRY(name)                                                                                \
    case irr::gui::EGET_##name:                                                                    \
        return "EGET_" #name;

    switch (type) {
        ENTRY(ELEMENT_FOCUS_LOST);
        ENTRY(ELEMENT_FOCUSED);
        ENTRY(ELEMENT_HOVERED);
        ENTRY(ELEMENT_LEFT);
        ENTRY(ELEMENT_CLOSED);
        ENTRY(BUTTON_CLICKED);
        ENTRY(SCROLL_BAR_CHANGED);
        ENTRY(CHECKBOX_CHANGED);
        ENTRY(LISTBOX_CHANGED);
        ENTRY(LISTBOX_SELECTED_AGAIN);
        ENTRY(FILE_SELECTED);
        ENTRY(DIRECTORY_SELECTED);
        ENTRY(FILE_CHOOSE_DIALOG_CANCELLED);
        ENTRY(MESSAGEBOX_YES);
        ENTRY(MESSAGEBOX_NO);
        ENTRY(MESSAGEBOX_OK);
        ENTRY(MESSAGEBOX_CANCEL);
        ENTRY(EDITBOX_ENTER);
        ENTRY(EDITBOX_CHANGED);
        ENTRY(EDITBOX_MARKING_CHANGED);
        ENTRY(TAB_CHANGED);
        ENTRY(MENU_ITEM_SELECTED);
        ENTRY(COMBO_BOX_CHANGED);
        ENTRY(SPINBOX_CHANGED);
        ENTRY(TABLE_CHANGED);
        ENTRY(TABLE_HEADER_CHANGED);
        ENTRY(TABLE_SELECTED_AGAIN);
        ENTRY(TREEVIEW_NODE_DESELECT);
        ENTRY(TREEVIEW_NODE_SELECT);
        ENTRY(TREEVIEW_NODE_EXPAND);
        ENTRY(TREEVIEW_NODE_COLLAPSE);
    default:
        return std::string("Unknown GUI event type ") + std::to_string(type);
    }

#undef ENTRY
};

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

#endif /* end of include guard: GRAPHICS_GRAPHICS_HPP */
