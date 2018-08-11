#include <geometry/geometry.hpp>

irr::core::vector3df getRotationByTarget(const GamePosition& pos, const GamePosition& target) {
    auto vec = target.toIrrVector3df() - pos.toIrrVector3df();
    return vec.getHorizontalAngle();
}
