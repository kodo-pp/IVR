#ifndef GAME_SOLID_OBJECT_HPP
#define GAME_SOLID_OBJECT_HPP

#include <game/game_object.hpp>
#include <graphics/graphics.hpp>
#include <vector>

class SolidObjectPart : public GameObject {
public:
    SolidObjectPart();
};

// COMBAK: these properties should be statically linked to id, not to SolidObject
// instance
// POSSIBLE FIX: hold 'parts' only for incomplete game objects, when it is complete,
// set the 'complete' flag

class SolidObject : public GameObject {
public:
    SolidObject();
    bool isSeparable();
    std::vector <SolidObjectPart> & getParts();

protected:
    bool separable;

    // COMBAK [see above]: may cause performance issues and high RAM usage
// #warning possible performance issues here
    std::vector <SolidObjectPart> parts;
};

#endif /* end of include guard: GAME_SOLID_OBJECT_HPP */
