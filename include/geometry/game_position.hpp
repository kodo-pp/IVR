#ifndef GEOMETRY_GAME_POSITION_HPP
#define GEOMETRY_GAME_POSITION_HPP

/**
 * A simple class which represents (3D) position in the world
 *
 * The position is universal and can be used for player(s) or game objects.
 */

#include <iostream>

#include <irrlicht.h>
using namespace irr::core;

class GamePosition
{
public:
    GamePosition(double _x = 0.0, double _y = 0.0, double _z = 0.0);
    GamePosition(const GamePosition& gamePosition); // copy c-tor
    GamePosition(GamePosition&& gamePosition);      // move c-tor
    explicit GamePosition(const vector3df& irrvec);

    vector3df toIrrVector3df() const;

    GamePosition& operator=(const GamePosition& other);
    GamePosition& operator=(GamePosition&& other);

    GamePosition operator+(const GamePosition& other);
    GamePosition& operator+=(const GamePosition& other);

    bool operator==(const GamePosition& other) const;
    bool operator==(GamePosition&& other) const;

    bool operator!=(const GamePosition& other) const;
    bool operator!=(GamePosition&& other) const;

    std::pair<int64_t, int64_t> getChunk() const;

    // Yes, these are public
    // I think there is no need in getters and setters (as in std::pair)
    double x;
    double y;
    double z;
};

std::wostream& operator<<(std::wostream& out, const GamePosition& pos);

#endif /* end of include guard: GEOMETRY_GAME_POSITION_HPP */
