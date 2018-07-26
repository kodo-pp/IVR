#ifndef INCLUDE_GAME_PLAYER_HPP
#define INCLUDE_GAME_PLAYER_HPP

#include <geometry/game_position.hpp>
#include <graphics/graphics.hpp>

class Player {
public:
    Player(irr::scene::ICameraSceneNode* _camera);
    Player(const Player& other) = default;
    Player(Player&& other) = default;
    virtual ~Player() = default;

    Player& operator=(const Player& other) = default;
    Player& operator=(Player&& other) = default;

    void move(double dx, double dz);
    void moveForward(double delta, double directionOffset);
    void turn(double dx, double dy);
    void jump(double speed);

private:
    irr::scene::ICameraSceneNode* camera;
    irr::core::vector3df rotation;
};

#endif /* end of include guard: INCLUDE_GAME_PLAYER_HPP */
