#ifndef GAME_GAME_OBJECT_HPP
#define GAME_GAME_OBJECT_HPP

#include <string>
#include <geometry/game_position.hpp>

using GameObjectId = uint64_t;

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
    GameObjectId id;
    ModuleId providingModule;
};

#endif /* end of include guard: GAME_GAME_OBJECT_HPP */
