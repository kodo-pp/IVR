#ifndef GEOMETRY_GAME_POSITION_HPP
#define GEOMETRY_GAME_POSITION_HPP

/**
 * A simple class which represents (3D) position in the world
 *
 * The position is universal and can be used for player(s) or game objects.
 */

class GamePosition {
public:
    GamePosition(double _x = 0.0, double _y = 0.0, double _z = 0.0):
            x(_x), y(_y), z(_z) { }
    GamePosition(GamePosition& gamePosition); // copy c-tor
    GamePosition(GamePosition&& gamePosition); // move c-tor

    // TODO: put this to game_position.cpp
/*
    bool operator ==(const GamePosition& other) {
        return x == other.x && y == other.y && z == other.z;
    }
    bool operator ==(GamePosition&& other) {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator !=(const GamePosition& other) {
        return x != other.x || y != other.y || z != other.z;
    }
    bool operator !=(GamePosition&& other) {
        return x != other.x || y != other.y || z != other.z;
    }
*/
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
