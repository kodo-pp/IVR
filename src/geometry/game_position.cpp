#include <tuple>

#include <modbox/geometry/game_position.hpp>
#include <modbox/world/terrain.hpp>

GamePosition::GamePosition(double _x, double _y, double _z) : x(_x), y(_y), z(_z)
{
}

GamePosition::GamePosition(const GamePosition& other)
{
    std::tie(x, y, z) = std::make_tuple(other.x, other.y, other.z);
}
GamePosition::GamePosition(GamePosition&& other)
{
    std::tie(x, y, z) = std::make_tuple(other.x, other.y, other.z);
}
GamePosition::GamePosition(const vector3df& irrvec)
{
    std::tie(x, y, z) = std::make_tuple(irrvec.X, irrvec.Y, irrvec.Z);
}

vector3df GamePosition::toIrrVector3df() const
{
    return vector3df(x, y, z);
}

GamePosition& GamePosition::operator=(const GamePosition& other)
{
    std::tie(x, y, z) = std::make_tuple(other.x, other.y, other.z);
    return *this;
}
GamePosition& GamePosition::operator=(GamePosition&& other)
{
    std::tie(x, y, z) = std::make_tuple(other.x, other.y, other.z);
    return *this;
}

bool GamePosition::operator==(const GamePosition& other) const
{
    return x == other.x && y == other.y && z == other.z;
}
bool GamePosition::operator==(GamePosition&& other) const
{
    return x == other.x && y == other.y && z == other.z;
}

bool GamePosition::operator!=(const GamePosition& other) const
{
    return x != other.x || y != other.y || z != other.z;
}
bool GamePosition::operator!=(GamePosition&& other) const
{
    return x != other.x || y != other.y || z != other.z;
}

GamePosition GamePosition::operator+(const GamePosition& other)
{
    return GamePosition(x + other.x, y + other.y, z + other.z);
}
GamePosition& GamePosition::operator+=(const GamePosition& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

std::pair<int64_t, int64_t> GamePosition::getChunk() const
{
    return {floor(x) / CHUNK_SIZE_IRRLICHT, floor(z) / CHUNK_SIZE_IRRLICHT};
}

std::wostream& operator<<(std::wostream& out, const GamePosition& pos)
{
    out << L"GamePosition(" << pos.x << L", " << pos.y << L", " << pos.z << L")";
    return out;
}
