#include <game/game_object.hpp>
#include <graphics/graphics.hpp>
#include <unordered_map>

std::unordered_map <uint64_t, GameObject*> gameObjects;

uint64_t registerGameObject(GameObject* obj) {
    auto idx = static_cast <uint64_t> (gameObjects.size());
    gameObjects.insert(std::make_pair(idx, obj));
    return obj;
}

GameObject* getGameObject(uint64_t idx) {
    return gameObjects.at(idx);
}

void unregisterGameObject(uint64_t idx) {
    gameObjects.erase(gameObjects.find(idx));
}

GameObject::GameObject() { }
GameObject::GameObject(std::wstring meshFilename) {
    _sceneNode = graphicsCreateObject(meshFilename);
}

GameObject::GameObject(ISceneNode* node) _sceneNode(node) { }

GameObject::~GameObject() { }

GamePosition GameObject::getPosition() {
    return position;
}

void GameObject::setPosition(GamePosition newPosition) {
    position = newPosition;
    graphicsMoveObject(_sceneNode, newPosition);
}

ISceneNode* GameObject::sceneNode() {
    return _sceneNode;
}
