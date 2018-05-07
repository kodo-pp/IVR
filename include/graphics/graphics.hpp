#ifndef GRAPHICS_GRAPHICS_HPP
#define GRAPHICS_GRAPHICS_HPP

#include <vector>
#include <string>
#include <geometry/game_position.hpp>
#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace video;

bool initializeGraphics(std::vector <std::string> *);

ISceneNode* graphicsCreateObject(const std::wstring&);
ISceneNode* graphicsCreateCube();

void graphicsMoveObject(ISceneNode*, double, double, double);
void graphicsMoveObject(ISceneNode*, core::vector3df);
void graphicsMoveObject(ISceneNode*, GamePosition);

ITexture* graphicsLoadTexture(std::wstring);
void graphicsAddTexture(ISceneNode*, ITexture*);

void graphicsDraw();

#endif /* end of include guard: GRAPHICS_GRAPHICS_HPP */
