#include <cmath>
#include <game/player.hpp>
#include <geometry/game_position.hpp>
#include <geometry/geometry.hpp>
#include <graphics/graphics.hpp>
#include <irrlicht.h>
#include <log/log.hpp>
#include <misc/irrvec.hpp>
#include <tuple>

Player::Player(irr::scene::ICameraSceneNode* _camera) : camera(_camera) {}

void Player::move(double dx, double dz) {
    camera->updateAbsolutePosition();
    camera->setPosition(camera->getPosition() + irr::core::vector3df(dx, 0, dz));
    camera->updateAbsolutePosition();
}

void Player::moveForward(double delta, double directionOffset) {
    // XXX: For now, vertical camera angle is ignored. Maybe it should be so,
    // maybe it should not
    double x, y, z;
    std::tie(x, y, z) = irrvec_extract(camera->getRotation());
    double azimuth = getAzimuth(x, y, z) + directionOffset;
    double deltaX = delta * sin(azimuth);
    double deltaZ = delta * cos(azimuth);
    move(deltaX, deltaZ);
}

static bool checkCameraVerticalOverrotation(const irr::core::vector3df& rot) {
    LOG(rot.X);
    return -90 <= rot.X && rot.X <= 90;
}

void Player::turn(double dx, double dy) {
    camera->setRotation(rotation);
    camera->updateAbsolutePosition();
    auto currentRotation = camera->getRotation();
    if (!checkCameraVerticalOverrotation(currentRotation)) {
        LOG("No");
        return;
    }
    camera->setRotation(currentRotation + irr::core::vector3df(dx, dy, 0));
    camera->updateAbsolutePosition();
    rotation = camera->getRotation();
}

void Player::jump(double speed) {
    auto anim = static_cast<irr::scene::ISceneNodeAnimatorCollisionResponse*>(
            *camera->getAnimators().begin());
    if (!anim->isFalling()) {
        anim->jump(speed);
    }
}