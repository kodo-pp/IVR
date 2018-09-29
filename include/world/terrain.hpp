#ifndef WORLD_TERRAIN_HPP
#define WORLD_TERRAIN_HPP

#include <map>
#include <string>
#include <vector>

#include <modbox/game/enemy.hpp>
#include <modbox/world/chunk.hpp>

#include <irrlicht.h>

const double CHUNK_SIZE_IRRLICHT = 2400;

class TerrainManager
{
public:
    using offset_t = int64_t;
    TerrainManager() = default;
    TerrainManager(const TerrainManager& other) = delete;
    TerrainManager(TerrainManager&& other) = default;
    virtual ~TerrainManager() = default;

    TerrainManager& operator=(const TerrainManager& other) = delete;
    TerrainManager& operator=(TerrainManager&& other) = default;

    void loadTerrain(offset_t off_x, offset_t off_y);
    const Chunk& getChunk(offset_t off_x, offset_t off_y) const;
    Chunk& getMutableChunk(offset_t off_x, offset_t off_y);
    void addChunk(offset_t off_x, offset_t off_y, const Chunk& chunk);
    void addChunk(offset_t off_x, offset_t off_y, Chunk&& chunk);
    void deleteChunk(offset_t off_x, offset_t off_y);

    void trackMob(EnemyId mobId);
    void updateMob(EnemyId mobId);
    void forgetMob(EnemyId mobId);

    void mobsAi();

    void trackObject(GameObjectId objectId);
    void updateObject(GameObjectId objectId);
    void forgetObject(GameObjectId objectId);

private:
    // XXX: maybe replace with unordered_map, but then wee need to write hash() function for
    // std::pair, which may not be a trivial task
    std::map<std::pair<offset_t, offset_t>, Chunk> chunks;

    std::map<EnemyId, std::pair<offset_t, offset_t>> enemies;
    std::map<GameObjectId, std::pair<offset_t, offset_t>> objects;
};

extern TerrainManager terrainManager;

#endif /* end of include guard: WORLD_TERRAIN_HPP */
