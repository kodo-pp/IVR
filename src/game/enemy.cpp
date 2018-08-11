#include <game/enemy.hpp>
#include <geometry/game_position.hpp>
#include <graphics/graphics.hpp>
#include <util/util.hpp>

Enemy::Enemy(irr::scene::ISceneNode* _node) : node(_node) {}

void Enemy::hit(UNUSED double damage) {}

double Enemy::getHealthLeft() {
    return healthLeft;
}

void Enemy::setHealthLeft(double health) {
    healthLeft = health;
}

double Enemy::getHealthMax() {
    return healthMax;
}
void Enemy::setHealthMax(double health) {
    healthMax = health;
}

GamePosition Enemy::getPosition() {
    return position;
}
void Enemy::setPosition(const GamePosition& newPosition) {
    position = newPosition;
}

irr::scene::ISceneNode* Enemy::sceneNode() {
    return node;
}

Enemy createEnemy(irr::scene::IMesh* model, irr::video::ITexture* texture) {
    Enemy enemy(graphicsCreateMeshSceneNode(model));
    enemy.sceneNode()->setMaterialTexture(0, texture);
    return enemy;
}
