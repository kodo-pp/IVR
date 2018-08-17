#include <game/enemy.hpp>
#include <geometry/game_position.hpp>
#include <geometry/geometry.hpp>
#include <graphics/graphics.hpp>
#include <util/util.hpp>

Enemy::Enemy(irr::scene::ISceneNode* _node, const std::function<GamePosition(void)>& _ai_func)
        : node(_node), ai_func(_ai_func), movementSpeed(3) {}

void Enemy::hit(double damage) {
    healthLeft -= damage;
}

double Enemy::getHealthLeft() const {
    return healthLeft;
}

void Enemy::setHealthLeft(double health) {
    healthLeft = health;
}

double Enemy::getHealthMax() const {
    return healthMax;
}
void Enemy::setHealthMax(double health) {
    healthMax = health;
}

GamePosition Enemy::getPosition() const {
    position = GamePosition(node->getPosition());
    return position;
}
void Enemy::setPosition(const GamePosition& newPosition) {
    position = newPosition;
    node->setPosition(newPosition.toIrrVector3df());
}

double Enemy::getMovementSpeed() const {
    return movementSpeed;
}
void Enemy::setMovementSpeed(double newSpeed) {
    movementSpeed = newSpeed;
}

irr::scene::ISceneNode* Enemy::sceneNode() const {
    return node;
}

void Enemy::ai() {
    auto target = ai_func();
    auto self_position = getPosition();
    auto vec = (target.toIrrVector3df() - self_position.toIrrVector3df());
    vec.Y = 0;

    const double treshold = 5;
    if (vec.getLengthSQ() < treshold) {
        movementSpeed = 0;
    }

    vec = vec.normalize() * movementSpeed;
    auto rotation = getRotationByTarget(getPosition(), ai_func());
    rotation.X = 0;
    rotation.Z = 0;
    node->setRotation(rotation);
    setPosition(GamePosition(self_position.toIrrVector3df() + vec));
}

EnemyId EnemyManager::createEnemy(irr::scene::IMesh* model,
                                  irr::video::ITexture* texture,
                                  const irr::core::vector3df& scale) {
    ++idCounter;
    enemies.insert({idCounter, Enemy(graphicsCreateMeshSceneNode(model))});

    Enemy& enemy = enemies.at(idCounter);
    enemy.sceneNode()->setMaterialTexture(0, texture);
    enemy.sceneNode()->setScale(scale);
    return idCounter;
}

void EnemyManager::deleteEnemy(EnemyId id) {
    enemies.erase(id);
}

const Enemy& EnemyManager::accessEnemy(EnemyId id) {
    return enemies.at(id);
}
Enemy& EnemyManager::mutableAccessEnemy(EnemyId id) {
    return enemies.at(id);
}

EnemyManager enemyManager;
