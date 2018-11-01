#ifndef GAME_ENEMY_HPP
#define GAME_ENEMY_HPP

#include <functional>
#include <unordered_map>

#include <modbox/game/ai.hpp>
#include <modbox/geometry/game_position.hpp>
#include <mutex>

using EnemyId = uint64_t;

// XXX: maybe rename to Mob

/**
 * Represents an abstract enemy (mob)
 */

class Enemy
{
public:
    Enemy(irr::scene::ISceneNode* _node, const std::string& _kind, EnemyId _id);
    Enemy(const Enemy& other) = default;
    Enemy(Enemy&& other) = default;
    virtual ~Enemy() = default;

    Enemy& operator=(const Enemy& other) = default;
    Enemy& operator=(Enemy&& other) = default;

    virtual void hit(double damage);

    double getHealthLeft() const;
    void setHealthLeft(double health);

    double getHealthMax() const;
    void setHealthMax(double health);

    std::string getKind() const;

    GamePosition getPosition() const;
    void setPosition(const GamePosition& newPosition);

    bool isDead() const;

    irr::scene::ISceneNode* sceneNode() const;

    virtual void ai();

protected:
    EnemyId id;
    std::string kind;
    double movementSpeed;
    double healthLeft;
    double healthMax;
    irr::scene::ISceneNode* node;
};

class EnemyManager
{
public:
    EnemyManager() = default;
    EnemyManager(const EnemyManager& other) = default;
    EnemyManager(EnemyManager&& other) = default;
    virtual ~EnemyManager() = default;

    EnemyManager& operator=(const EnemyManager& other) = default;
    EnemyManager& operator=(EnemyManager&& other) = default;

    EnemyId createEnemy(const std::string& kind, irr::scene::ISceneNode* model);
    void deleteEnemy(EnemyId id);
    const Enemy& accessEnemy(EnemyId id);
    Enemy& mutableAccessEnemy(EnemyId id);

    void addKind(
        const std::string& kind,
        const std::function <void(EnemyId)>& creationFunction,
        const std::function <std::string(EnemyId)>& aiFunction
    );
    std::function<std::string(EnemyId)> getAiFunction(const std::string& kind);

private:
    mutable std::recursive_mutex mutex;
    std::unordered_map<std::string, std::function<std::string(EnemyId)>> aiFunctionsByKind;
    std::unordered_map<std::string, std::function<void(EnemyId)>> creationFunctionsByKind;
    std::unordered_map<EnemyId, Enemy> enemies;
    EnemyId idCounter = 0;
};

extern EnemyManager enemyManager;

void initializeEnemies();

#endif /* end of include guard: GAME_ENEMY_HPP */
