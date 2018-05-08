#ifndef GAME_GAME_OBJECT_HPP
#define GAME_GAME_OBJECT_HPP

#include <string>
#include <geometry/game_position.hpp>
#include <graphics/graphics.hpp>
#include <modules/module.hpp>
#include <string>

using GameObjectId = uint64_t;

/**
 * An class which represents an abstract game object
 *
 * WARNING: instantiate not this class but its derived classes
 */
class GameObject {
public:
    GameObject();
    GameObject(std::wstring);
    virtual ~GameObject();

    GamePosition getPosition();
    void setPosition(GamePosition newPosition);

    ISceneNode* sceneNode();

protected:
    GamePosition position;
    GameObjectId id;
    ModuleId providingModule;

    ISceneNode* _sceneNode;
};

#endif /* end of include guard: GAME_GAME_OBJECT_HPP */
