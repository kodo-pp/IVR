#include <mutex>
#include <unordered_map>

#include <game/game_object.hpp>
#include <graphics/graphics.hpp>
#include <log/log.hpp>
#include <util/handle_storage.hpp>

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

// TODO: replace copy-paste driven programming with using copy-and-swap idiom

GameObject::GameObject(const GameObject& other)
{
    _sceneNode = other.sceneNode();
    position = other.getPosition();
    providingModule = other.getProvidingModule();
    id = other.getId();
}

GameObject::GameObject(GameObject&& other)
{
    std::swap(_sceneNode, other._sceneNode);
    std::swap(position, other.position);
    std::swap(providingModule, other.providingModule);
    std::swap(id, other.id);
}

GameObject& GameObject::operator=(const GameObject& other)
{
    _sceneNode = other.sceneNode();
    position = other.getPosition();
    providingModule = other.getProvidingModule();
    id = other.getId();
    return *this;
}

GameObject& GameObject::operator=(GameObject&& other)
{
    std::swap(_sceneNode, other._sceneNode);
    std::swap(position, other.position);
    std::swap(providingModule, other.providingModule);
    std::swap(id, other.id);
    return *this;
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
ModuleId GameObject::getProvidingModule() const
{
    return providingModule;
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
