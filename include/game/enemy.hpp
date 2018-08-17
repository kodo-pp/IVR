#ifndef GAME_ENEMY_HPP
#define GAME_ENEMY_HPP

#include <functional>
#include <game/ai.hpp>
#include <geometry/game_position.hpp>
#include <unordered_map>

using EnemyId = uint64_t;

// XXX: maybe rename to Mob

/**
 * Represents an abstract enemy (mob)
 */

class Enemy {
public:
    Enemy(irr::scene::ISceneNode* _node,
          const std::function<GamePosition(void)>& _ai_func = getDefaultAiFunc());
    Enemy(const Enemy& other) = delete;
    Enemy(Enemy&& other) = default;
    virtual ~Enemy() = default;

    Enemy& operator=(const Enemy& other) = delete;
    Enemy& operator=(Enemy&& other) = default;

    virtual void hit(double damage);

    double getHealthLeft() const;
    void setHealthLeft(double health);

    double getHealthMax() const;
    void setHealthMax(double health);

    double getMovementSpeed() const;
    void setMovementSpeed(double newSpeed);

    GamePosition getPosition() const;
    void setPosition(const GamePosition& newPosition);

    irr::scene::ISceneNode* sceneNode() const;

    void ai();

protected:
    double healthLeft;
    double healthMax;
    mutable GamePosition position;
    irr::scene::ISceneNode* node;
    std::function<GamePosition(void)> ai_func;
    double movementSpeed;
};

class EnemyManager {
public:
    EnemyManager() = default;
    EnemyManager(const EnemyManager& other) = default;
    EnemyManager(EnemyManager&& other) = default;
    virtual ~EnemyManager() = default;

    EnemyManager& operator=(const EnemyManager& other) = default;
    EnemyManager& operator=(EnemyManager&& other) = default;

    EnemyId createEnemy(irr::scene::IMesh* model,
                        irr::video::ITexture* texture,
                        const irr::core::vector3df& scale = {1, 1, 1});
    void deleteEnemy(EnemyId id);
    const Enemy& accessEnemy(EnemyId id);
    Enemy& mutableAccessEnemy(EnemyId id);

private:
    std::unordered_map<EnemyId, Enemy> enemies;
    EnemyId idCounter = 0;
};

extern EnemyManager enemyManager;

#endif /* end of include guard: GAME_ENEMY_HPP */
