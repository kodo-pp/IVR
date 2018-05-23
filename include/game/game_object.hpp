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
    GameObject(ISceneNode*);
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

uint64_t registerGameObject(GameObject* obj);
GameObject* getGameObject(uint64_t idx);
void unregisterGameObject(uint64_t idx);

#endif /* end of include guard: GAME_GAME_OBJECT_HPP */
