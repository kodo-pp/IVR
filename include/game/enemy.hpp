#ifndef GAME_ENEMY_HPP
#define GAME_ENEMY_HPP

#include <geometry/game_position.hpp>

// XXX: maybe rename to Mob

/**
 * Represents an abstract enemy (mob)
 */

class Enemy {
public:
    // XXX: maybe c-tor should exist
    Enemy() = delete;
    virtual ~Enemy();

    void hit(double damage);

    double getHealthLeft();
    void setHealthLeft(double health);

    double getHealthMax();
    void setHealthMax(double health);

    GamePosition getPosition();
    void setPosition(GamePosition newPosition);

protected:
    double healthLeft;
    double healthMax;
    GamePosition position;
};

#endif /* end of include guard: GAME_ENEMY_HPP */
