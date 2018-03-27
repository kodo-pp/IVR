#ifndef GAME_GAME_OBJECT_HPP
#define GAME_GAME_OBJECT_HPP

#include <geometry/game_position.hpp>

/**
 * An class which represents an abstract game object
 *
 * WARNING: instantiate not this class but its derived classes
 */
class GameObject {
public:
    GameObject() = delete;
    virtual ~GameObject();

    GamePosition getPosition();
    void setPosition(GamePosition newPosition);

protected:
    GamePosition position;
};

#endif /* end of include guard: GAME_GAME_OBJECT_HPP */
