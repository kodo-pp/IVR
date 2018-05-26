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
GameObjCube&& graphicsCreateCube();

void graphicsMoveObject(ISceneNode*, double, double, double);
void graphicsMoveObject(ISceneNode*, core::vector3df);
void graphicsMoveObject(ISceneNode*, GamePosition);

ITexture* graphicsLoadTexture(std::wstring);
void graphicsAddTexture(const GameObject&, ITexture*);

void graphicsDraw();

#endif /* end of include guard: GRAPHICS_GRAPHICS_HPP */
