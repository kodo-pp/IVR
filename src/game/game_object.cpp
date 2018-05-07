#include <game/game_object.hpp>
#include <graphics/graphics.hpp>

GameObject::GameObject() { }
GameObject::GameObject(std::wstring meshFilename) {
    _sceneNode = graphicsCreateObject(meshFilename);
}

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
