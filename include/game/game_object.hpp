#ifndef GAME_GAME_OBJECT_HPP
#define GAME_GAME_OBJECT_HPP

#include <string>

#include <modbox/geometry/game_position.hpp>

// XXX: очередной костыль
//#include <graphics/graphics.hpp>
#include <mutex>

#include <modbox/modules/module.hpp>

#include <irrlicht.h>

using irr::scene::ISceneNode;

using GameObjectId = uint64_t;

extern std::recursive_mutex gameObjectMutex;

/**
 * A class which represents an abstract game object
 *
 * WARNING: instantiate not this class but its derived classes
 */
class GameObject
{
public:
    GameObject();
    GameObject(const GameObject&) = default;
    GameObject(GameObject&&) = default;
    GameObject(ISceneNode*);
    virtual ~GameObject();

    GamePosition getPosition() const;
    void setPosition(const GamePosition& newPosition);
    void setRotation(double, double, double);

    GameObject& operator=(const GameObject&) = default;
    GameObject& operator=(GameObject&&) = default;

    ISceneNode* sceneNode() const;
    GameObjectId getId() const;

    bool getPhysicsEnabled();
    void setPhysicsEnabled(bool value);

protected:
    bool physicsEnabled;
    GamePosition position;
    GameObjectId id;

    ISceneNode* _sceneNode;
};

uint64_t registerGameObject(GameObject* obj);
GameObject* getGameObject(uint64_t idx);
void unregisterGameObject(uint64_t idx);

#endif /* end of include guard: GAME_GAME_OBJECT_HPP */
