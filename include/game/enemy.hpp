#ifndef GAME_ENEMY_HPP
#define GAME_ENEMY_HPP

#include <geometry/game_position.hpp>

// XXX: maybe rename to Mob

/**
 * Represents an abstract enemy (mob)
 */

class Enemy {
public:
    Enemy(irr::scene::ISceneNode* _node);
    Enemy(const Enemy& other) = default;
    Enemy(Enemy&& other) = default;
    virtual ~Enemy() = default;

    Enemy& operator=(const Enemy& other) = default;
    Enemy& operator=(Enemy&& other) = default;

    virtual void hit(double damage);

    double getHealthLeft();
    void setHealthLeft(double health);

    double getHealthMax();
    void setHealthMax(double health);

    GamePosition getPosition();
    void setPosition(const GamePosition& newPosition);

    irr::scene::ISceneNode* sceneNode();

protected:
    double healthLeft;
    double healthMax;
    GamePosition position;
    irr::scene::ISceneNode* node;
};

Enemy createEnemy(irr::scene::IMesh* model, irr::video::ITexture* texture);

#endif /* end of include guard: GAME_ENEMY_HPP */
