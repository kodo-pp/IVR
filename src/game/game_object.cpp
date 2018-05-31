#include <game/game_object.hpp>
#include <graphics/graphics.hpp>
#include <unordered_map>
#include <log/log.hpp>
std::unordered_map <uint64_t, GameObject*> gameObjects;

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
    LOG("GameObject: copy constructor");
    _sceneNode = other.sceneNode();
    position = other.getPosition();
    providingModule = other.getProvidingModule();
    id = other.getId();
}

GameObject::GameObject(GameObject&& other) {
    LOG("GameObject: move constructor");
    std::swap(_sceneNode, other._sceneNode);
    std::swap(position, other.position);
    std::swap(providingModule, other.providingModule);
    std::swap(id, other.id);
}

GameObject& GameObject::operator =(const GameObject& other) {
    LOG("GameObject: copy operator =");
    _sceneNode = other.sceneNode();
    position = other.getPosition();
    providingModule = other.getProvidingModule();
    id = other.getId();
    return *this;
}

GameObject& GameObject::operator =(GameObject&& other) {
    LOG("GameObject: move operator =");
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

ISceneNode* GameObject::sceneNode() const {
    return _sceneNode;
}
