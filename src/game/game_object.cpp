#include <mutex>
#include <unordered_map>

#include <modbox/game/game_object.hpp>
#include <modbox/graphics/graphics.hpp>
#include <modbox/log/log.hpp>
#include <modbox/util/handle_storage.hpp>

HandleStorage<uint64_t, GameObject*> gameObjects;
std::recursive_mutex gameObjectMutex;

uint64_t registerGameObject(GameObject* obj)
{
    return gameObjects.insert(obj);
}

GameObject* getGameObject(uint64_t idx)
{
    // TODO: may there be a situation in which we'd like to use mutableAccess?
    return gameObjects.access(idx);
}

void unregisterGameObject(uint64_t idx)
{
    gameObjects.remove(idx);
}

GameObject::GameObject() : physicsEnabled(false)
{
}

/*
GameObject::GameObject(std::wstring meshFilename) {
    _sceneNode = graphicsCreateObject(meshFilename);
}
*/

GameObject::GameObject(ISceneNode* node) : _sceneNode(node)
{
}

GameObject::~GameObject()
{
}

GamePosition GameObject::getPosition() const
{
    return position;
}
GameObjectId GameObject::getId() const
{
    return id;
}
void GameObject::setPosition(const GamePosition& newPosition)
{
    position = newPosition;
    graphicsMoveObject(_sceneNode, newPosition);
}

void GameObject::setRotation(double pitch, double roll, double yaw)
{
    graphicsRotateObject(_sceneNode, irr::core::vector3df(pitch, roll, yaw));
}

ISceneNode* GameObject::sceneNode() const
{
    return _sceneNode;
}

bool GameObject::getPhysicsEnabled()
{
    return physicsEnabled;
}
void GameObject::setPhysicsEnabled(bool value)
{
    if (value) {
        graphicsEnablePhysics(_sceneNode);
    } else {
        position = GamePosition(_sceneNode->getPosition());
        graphicsDisablePhysics(_sceneNode);
    }
    physicsEnabled = value;
}
