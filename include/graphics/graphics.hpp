#ifndef GRAPHICS_GRAPHICS_HPP
#define GRAPHICS_GRAPHICS_HPP

#include <game/objects/objects.hpp>
#include <geometry/game_position.hpp>
#include <vector>
#include <string>
#include <unordered_set>
#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace video;

// ===== Initialization and cleanup =====

/// Initialize graphical engine and related data structures
/// Should be called once when the game is started
bool initializeGraphics(std::vector <std::string> *);

/// Stop graphical engine and free related resources
/// Should be called once when the game is exited
void cleanupGraphics();


// ===== Managing objects =====

/// Create a cube and return the GameObject instance
GameObjCube graphicsCreateCube();

/// Move a scene node to the specified location
void graphicsMoveObject(ISceneNode*, double, double, double);
void graphicsMoveObject(ISceneNode*, core::vector3df);
void graphicsMoveObject(ISceneNode*, GamePosition);

/// Rotate a scene node
void graphicsRotateObject(ISceneNode* obj, core::vector3df rot);

/// Delete and unregister a game object
void graphicsDeleteObject(GameObject*);


// ===== Managing textures =====

/// Load texture and return Irrlicht ITexture* instance for this texture
ITexture* graphicsLoadTexture(std::wstring);

/// Add texture to a game object
void graphicsAddTexture(const GameObject&, ITexture*);


// ===== Drawing =====

/// Draw all necessary objects
void graphicsDraw();


// ===== Moving and rotating camera =====

/// Move camera to the specified position (...To())
/// or by a specified amount (...Delta())
void graphicsMoveCameraTo(const GamePosition& newPos);
void graphicsMoveCameraDelta(const GamePosition& delta);
void graphicsMoveCameraTo(const core::vector3df& newPos);
void graphicsMoveCameraDelta(const core::vector3df& delta);
void graphicsMoveCameraTo(double x, double y, double z);
void graphicsMoveCameraDelta(double dx, double dy, double dz);

/// Move the cmera forward (actual direction depends on the camera rotation)
/// by a specified amount
void graphicsMoveCameraForward(double delta, double directionOffset = 0);

/// Rotate camera
void graphicsRotateCamera(const core::vector3df& newRot);
void graphicsRotateCameraDelta(const core::vector3df& delta);
void graphicsRotateCamera(double x, double y, double z);
void graphicsRotateCameraDelta(double dx, double dy, double dz);


// ===== Utility functions =====

/// Poll for Ittlicht events related to window and similar stuff
/// Return false, if the window should be closed and false otherwise
bool irrDeviceRun();

// Irrlicht has terrible method and object member naming style
// But I actually have no choice... But wait, I have an idea
#define onEvent     OnEvent
#define keyInput    KeyInput
#define key         Key
#define pressedDown PressedDown
#define eventType   EventType
// Yeah, now it's much better

/// Event receiver for keyboard events
class IrrKeyboardEventReceiver : public irr::IEventReceiver {
public:
    IrrKeyboardEventReceiver() = default;
    virtual ~IrrKeyboardEventReceiver() = default;

    virtual bool onEvent(const irr::SEvent& event) override;
    virtual bool isKeyPressed(irr::EKEY_CODE key) const;

private:
    std::unordered_set <irr::EKEY_CODE> pressedKeys;
};
// And... Clean up
#undef onEvent
#undef keyInput
#undef key
#undef pressedDown
#undef eventType

const IrrKeyboardEventReceiver& getKeyboardEventReceiver();

#endif /* end of include guard: GRAPHICS_GRAPHICS_HPP */
