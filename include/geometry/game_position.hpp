#ifndef GEOMETRY_GAME_POSITION_HPP
#define GEOMETRY_GAME_POSITION_HPP

/**
 * A simple class which represents (3D) position in the world
 *
 * The position is universal and can be used for player(s) or game objects.
 */

#include <irrlicht.h>
using namespace irr::core;

class GamePosition {
public:
    GamePosition(double _x = 0.0, double _y = 0.0, double _z = 0.0);
    GamePosition(GamePosition& gamePosition); // copy c-tor
    GamePosition(GamePosition&& gamePosition); // move c-tor

    vector3df toIrrVector3df();

    GamePosition& operator=(const GamePosition& other);
    GamePosition& operator=(GamePosition&& other);

    bool operator ==(const GamePosition& other);
    bool operator ==(GamePosition&& other);

    bool operator !=(const GamePosition& other);
    bool operator !=(GamePosition&& other);

    // Yes, these are public
    // I think there is no need in getters and setters (as in std::pair)
    double x;
    double y;
    double z;
};

#endif /* end of include guard: GEOMETRY_GAME_POSITION_HPP */