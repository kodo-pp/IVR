#ifndef GRAPHICS_GRAPHICS_HPP
#define GRAPHICS_GRAPHICS_HPP

#include <vector>
#include <string>
#include <geometry/game_position.hpp>
#include <irrlicht.h>

using namespace irr;
using namespace scene;

bool initializeGraphics(std::vector <std::string> *);
ISceneNode* graphicsCreateObject(const std::wstring&);
void graphicsMoveObject(ISceneNode*, double, double, double);
void graphicsMoveObject(ISceneNode*, core::vector3df);
void graphicsMoveObject(ISceneNode*, GamePosition);
void graphicsDraw();

#endif /* end of include guard: GRAPHICS_GRAPHICS_HPP */
