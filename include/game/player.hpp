#ifndef INCLUDE_GAME_PLAYER_HPP
#define INCLUDE_GAME_PLAYER_HPP

#include <modbox/geometry/game_position.hpp>
#include <modbox/graphics/graphics.hpp>

class Player
{
public:
    Player(irr::scene::ICameraSceneNode* _camera, irr::scene::ISceneNode* _pseudoCamera);
    Player(const Player& other) = default;
    Player(Player&& other) = default;
    virtual ~Player() = default;

    Player& operator=(const Player& other) = default;
    Player& operator=(Player&& other) = default;

    void move(double dx, double dz);
    void moveForward(double delta, double directionOffset);
    void turn(double dx, double dy);
    void jump(double speed);

    GamePosition getPosition();
    core::vector3df getRotation();
    GamePosition getCameraTarget();

private:
    mutable std::recursive_mutex mutex;
    irr::scene::ICameraSceneNode* camera;
    irr::scene::ISceneNode* pseudoCamera;
    irr::core::vector3df rotation;
    irr::core::vector3df cachedPosition;
};

#endif /* end of include guard: INCLUDE_GAME_PLAYER_HPP */
