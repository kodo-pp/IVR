#ifndef GRAPHICS_GRAPHICS_HPP
#define GRAPHICS_GRAPHICS_HPP

#include <game/objects/objects.hpp>
#include <geometry/game_position.hpp>
#include <vector>
#include <string>
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

#endif /* end of include guard: GRAPHICS_GRAPHICS_HPP */
