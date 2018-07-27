#ifndef WORLD_WORLD_HPP
#define WORLD_WORLD_HPP

#include <geometry/game_position.hpp>
#include <map>
#include <set>
#include <string>
#include <world/chunk.hpp>

using WorldId = std::wstring;
using Seed = uint32_t; // XXX: maybe change to something different

// COMBAK: think about the logic of saving/loading chunks

/**
 * Represents the game world.
 */

class World {
public:
    // World(std::wstring _name, WorldId _id, std::wstring _path, Seed seed);
    World(std::map<std::wstring, std::wstring> config);

    std::map<ChunkId, Chunk> getChunks(std::set<ChunkId> _chunks);
    std::map<ChunkId, Chunk> getChunksRange(GamePosition pos, double range);

    void writeChunks(std::map<ChunkId, Chunk> _chunks); // XXX: not sure what should it do

    std::wstring getName();
    void setName(std::wstring newName);

    WorldId getId();
    // No setId() yet, because I think it should not be changed. However, I'm not
    // sure about it

    std::wstring getPath();
    void setPath(std::wstring newPath);

    Seed getSeed();

protected:
    Seed seed;
    std::wstring name;
    WorldId id;
    std::wstring path;
    std::map<ChunkId, Chunk> chunks;
};

#endif /* end of include guard: WORLD_WORLD_HPP */
