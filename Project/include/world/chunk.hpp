#ifndef WORLD_CHUNK_HPP
#define WORLD_CHUNK_HPP

#include <utility>
#include <map>
#include <game/game_object.hpp>
#include <geometry/game_position.hpp>

using ChunkId = std::pair <int64_t, int64_t>;

/**
 * Represents a chunk (a relatively small area in the world)
 *
 * Chunk has game objects inside, and they (game objects) can only be located
 * in chunks.
 */
class Chunk {
public:
    Chunk();
    // XXX: Maybe copy c-tor should be deleted
    explicit Chunk(Chunk& chunk); // Copy constructor
    explicit Chunk(Chunk&& chunk); // Move constructor
    explicit Chunk(std::unordered_map <GameObjectId, GameObject> _objects);

    ChunkId getId();
    // No setter as id should probably be read-only

protected:
    ChunkId id;
    std::unordered_map <GameObjectId, GameObject> objects;
};

#endif /* end of include guard: WORLD_CHUNK_HPP */
