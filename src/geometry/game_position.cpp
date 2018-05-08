#include <geometry/game_position.hpp>
#include <tuple>

GamePosition::GamePosition(double _x, double _y, double _z):
        x(_x), y(_y), z(_z) { }

GamePosition::GamePosition(GamePosition& other) {
    std::tie(x, y, z) = std::make_tuple(other.x, other.y, other.z);
}
GamePosition::GamePosition(GamePosition&& other) {
    std::tie(x, y, z) = std::make_tuple(other.x, other.y, other.z);
}

vector3df GamePosition::toIrrVector3df() {
    return vector3df(x, y, z);
}

GamePosition& GamePosition::operator=(const GamePosition& other) {
    std::tie(x, y, z) = std::make_tuple(other.x, other.y, other.z);
    return *this;
}
GamePosition& GamePosition::operator=(GamePosition&& other) {
    std::tie(x, y, z) = std::make_tuple(other.x, other.y, other.z);
    return *this;
}

bool GamePosition::operator ==(const GamePosition& other) {
    return x == other.x && y == other.y && z == other.z;
}
bool GamePosition::operator ==(GamePosition&& other) {
    return x == other.x && y == other.y && z == other.z;
}

bool GamePosition::operator !=(const GamePosition& other) {
    return x != other.x || y != other.y || z != other.z;
}
bool GamePosition::operator !=(GamePosition&& other) {
    return x != other.x || y != other.y || z != other.z;
}
