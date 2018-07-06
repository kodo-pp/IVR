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

bool initializeGraphics(std::vector <std::string> *);
void cleanupGraphics();

//ISceneNode* graphicsCreateObject(const std::wstring&);
GameObjCube graphicsCreateCube();

void graphicsMoveObject(ISceneNode*, double, double, double);
void graphicsMoveObject(ISceneNode*, core::vector3df);
void graphicsMoveObject(ISceneNode*, GamePosition);

void graphicsRotateObject(ISceneNode* obj, core::vector3df rot);

void graphicsDeleteObject(GameObject*);

ITexture* graphicsLoadTexture(std::wstring);
void graphicsAddTexture(const GameObject&, ITexture*);

void graphicsDraw();

void graphicsMoveCameraTo(const GamePosition& newPos);
void graphicsMoveCameraDelta(const GamePosition& delta);
void graphicsMoveCameraTo(const core::vector3df& newPos);
void graphicsMoveCameraDelta(const core::vector3df& delta);
void graphicsMoveCameraTo(double x, double y, double z);
void graphicsMoveCameraDelta(double dx, double dy, double dz);

void graphicsRotateCamera(const core::vector3df& newRot);
void graphicsRotateCameraDelta(const core::vector3df& delta);
void graphicsRotateCamera(double, double, double z);
void graphicsRotateCameraDelta(double dx, double dy, double dz);

bool irrDeviceRun();

// Irrlicht has terrible method and object member naming style
// But I actually have no choice... But wait, I have an idea
#define onEvent     OnEvent
#define keyInput    KeyInput
#define key         Key
#define pressedDown PressedDown
#define eventType   EventType
// Yeah, now it's much better
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
