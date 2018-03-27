#ifndef WORLD_WORLD_HPP
#define WORLD_WORLD_HPP

#include <string>
#include <set>
#include <map>
#include <world/chunk.hpp>
#include <geometry/game_position.hpp>

using WorldId = std::wstring;

/**
 * Represents the game world.
 */

class World {
public:
    World(std::wstring _name, WorldId _id, std::wstring _path);
    std::map <ChunkId, Chunk> getChunks(std::set <ChunkId> _chunks);
    std::map <ChunkId, Chunk> getChunksRange(GamePosition pos, double range);

    std::wstring getName();
    void setName(std::wstring newName);

    WorldId getId();
    // No setId() yet, because I think it should not be changed. However, I'm not
    // sure about it

    std::wstring getPath();
    void setPath(std::wstring newPath);

protected:
    std::wstring name;
    WorldId id;
    std::wstring path;
};

#endif /* end of include guard: WORLD_WORLD_HPP */
