#include <cmath>
#include <tuple>

#include <modbox/game/player.hpp>
#include <modbox/geometry/game_position.hpp>
#include <modbox/geometry/geometry.hpp>
#include <modbox/graphics/graphics.hpp>
#include <modbox/log/log.hpp>
#include <modbox/misc/irrvec.hpp>

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
    std::lock_guard<std::recursive_mutex> lock(mutex);
    pseudoCamera->setPosition(pseudoCamera->getPosition() + irr::core::vector3df(dx, 0, dz));
    // camera->updateAbsolutePosition();
    camera->setPosition(cachedPosition);
    // camera->updateAbsolutePosition();
    cachedPosition = pseudoCamera->getPosition();
}

void Player::moveForward(double delta, double directionOffset)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
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
    std::lock_guard<std::recursive_mutex> lock(mutex);
    // camera->setRotation(rotation);
    // camera->updateAbsolutePosition();
    // auto currentRotation = camera->getRotation();
    if (!checkCameraVerticalOverrotation(rotation + irr::core::vector3df(dx, dy, 0))) {
        return;
    }
    rotation += irr::core::vector3df(dx, dy, 0);
    // camera->bindTargetAndRotation(true);
    camera->updateAbsolutePosition();
    camera->setRotation(rotation);
    camera->updateAbsolutePosition();
}

void Player::jump(double speed)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    auto anim = static_cast<irr::scene::ISceneNodeAnimatorCollisionResponse*>(
            *pseudoCamera->getAnimators().begin());
    if (!anim->isFalling()) {
        anim->jump(speed);
    }
}

GamePosition Player::getPosition()
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return GamePosition(camera->getPosition());
}

core::vector3df Player::getRotation()
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return rotation;
}

GamePosition Player::getCameraTarget()
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return GamePosition(camera->getTarget());
}
