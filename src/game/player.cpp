#include <cmath>
#include <tuple>

#include <game/player.hpp>
#include <geometry/game_position.hpp>
#include <geometry/geometry.hpp>
#include <graphics/graphics.hpp>
#include <log/log.hpp>
#include <misc/irrvec.hpp>

#include <irrlicht.h>

Player::Player(irr::scene::ICameraSceneNode* _camera, irr::scene::ISceneNode* _pseudoCamera)
        : camera(_camera)
        , pseudoCamera(_pseudoCamera)
        , rotation({0, 0, 0})
        , cachedPosition(_pseudoCamera->getPosition())
{
}

void Player::move(double dx, double dz)
{
    pseudoCamera->setPosition(pseudoCamera->getPosition() + irr::core::vector3df(dx, 0, dz));
    camera->updateAbsolutePosition();
    camera->setPosition(cachedPosition);
    camera->updateAbsolutePosition();
    cachedPosition = pseudoCamera->getPosition();
}

void Player::moveForward(double delta, double directionOffset)
{
    // XXX: For now, vertical camera angle is ignored. Maybe it should be so,
    // maybe it should not
    double x, y, z;
    std::tie(x, y, z) = irrvec_extract(camera->getRotation());
    double azimuth = getAzimuth(x, y, z) + directionOffset;
    double deltaX = delta * sin(azimuth);
    double deltaZ = delta * cos(azimuth);
    move(deltaX, deltaZ);
}

static bool checkCameraVerticalOverrotation(const irr::core::vector3df& rot)
{
    const double eps = 1e-9;
    return -90 + eps <= rot.X && rot.X <= 90 - eps;
}

void Player::turn(double dx, double dy)
{
    camera->setRotation(rotation);
    camera->updateAbsolutePosition();
    auto currentRotation = camera->getRotation();
    if (!checkCameraVerticalOverrotation(currentRotation + irr::core::vector3df(dx, dy, 0))) {
        return;
    }
    camera->setRotation(currentRotation + irr::core::vector3df(dx, dy, 0));
    camera->updateAbsolutePosition();
    rotation = camera->getRotation();
}

void Player::jump(double speed)
{
    auto anim = static_cast<irr::scene::ISceneNodeAnimatorCollisionResponse*>(
            *pseudoCamera->getAnimators().begin());
    if (!anim->isFalling()) {
        anim->jump(speed);
    }
}

GamePosition Player::getPosition()
{
    return GamePosition(camera->getPosition());
}

core::vector3df Player::getRotation()
{
    return rotation;
}

GamePosition Player::getCameraTarget()
{
    return GamePosition(camera->getTarget());
}
