#ifndef GRAPHICS_GRAPHICS_HPP
#define GRAPHICS_GRAPHICS_HPP

#include <string>
#include <unordered_set>
#include <vector>

#include <game/objects/objects.hpp>
#include <geometry/game_position.hpp>
#include <util/handle_storage.hpp>

#include <boost/algorithm/string.hpp>
#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace video;

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

// ===== Managing textures =====

/// Load texture and return Irrlicht ITexture* instance for this texture
ITexture* graphicsLoadTexture(const std::wstring&);

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
                         const std::wstring& heightmap,
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
class IrrKeyboardEventReceiver : public irr::IEventReceiver
{
public:
    IrrKeyboardEventReceiver() = default;
    virtual ~IrrKeyboardEventReceiver() = default;

    virtual bool onEvent(const irr::SEvent& event) override;
    virtual bool isKeyPressed(irr::EKEY_CODE key) const;

private:
    std::unordered_set<irr::EKEY_CODE> pressedKeys;
};
// And... Clean up
#undef onEvent
#undef keyInput
#undef key
#undef pressedDown
#undef eventType

const IrrKeyboardEventReceiver& getKeyboardEventReceiver();

scene::ISceneNode* graphicsCreateMeshSceneNode(scene::IMesh* mesh);
scene::IMesh* graphicsLoadMesh(const std::wstring& filename);

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

#endif /* end of include guard: GRAPHICS_GRAPHICS_HPP */
