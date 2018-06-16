#include <game/game_object.hpp>
#include <graphics/graphics.hpp>
#include <log/log.hpp>
#include <unordered_map>
#include <mutex>

std::unordered_map <uint64_t, GameObject*> gameObjects;
std::recursive_mutex gameObjectMutex;

uint64_t registerGameObject(GameObject* obj) {
    //auto idx = static_cast <uint64_t> (gameObjects.size());
    uint64_t idx = 0;
    while (gameObjects.count(idx) != 0) {
        ++idx;
    }
    gameObjects.insert(std::make_pair(idx, obj));
    return idx;
}

GameObject* getGameObject(uint64_t idx) {
    return gameObjects.at(idx);
}

void unregisterGameObject(uint64_t idx) {
    gameObjects.erase(gameObjects.find(idx));
}

GameObject::GameObject() { }

// TODO: replace copy-paste driven programming with using copy-and-swap idiom

GameObject::GameObject(const GameObject& other) {
    _sceneNode = other.sceneNode();
    position = other.getPosition();
    providingModule = other.getProvidingModule();
    id = other.getId();
}

GameObject::GameObject(GameObject&& other) {
    std::swap(_sceneNode, other._sceneNode);
    std::swap(position, other.position);
    std::swap(providingModule, other.providingModule);
    std::swap(id, other.id);
}

GameObject& GameObject::operator =(const GameObject& other) {
    _sceneNode = other.sceneNode();
    position = other.getPosition();
    providingModule = other.getProvidingModule();
    id = other.getId();
    return *this;
}

GameObject& GameObject::operator =(GameObject&& other) {
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

GameObject::GameObject(ISceneNode* node): _sceneNode(node) { }

GameObject::~GameObject() { }

GamePosition GameObject::getPosition() const {
    return position;
}
GameObjectId GameObject::getId() const {
    return id;
}
ModuleId GameObject::getProvidingModule() const {
    return providingModule;
}

void GameObject::setPosition(GamePosition newPosition) {
    position = newPosition;
    graphicsMoveObject(_sceneNode, newPosition);
}

void GameObject::setRotation(double pitch, double roll, double yaw) {
    graphicsRotateObject(_sceneNode, irr::core::vector3df(pitch, roll, yaw));
}

ISceneNode* GameObject::sceneNode() const {
    return _sceneNode;
}
