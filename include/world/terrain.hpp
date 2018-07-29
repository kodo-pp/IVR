#ifndef WORLD_TERRAIN_HPP
#define WORLD_TERRAIN_HPP

#include <irrlicht.h>
#include <map>
#include <string>

const double CHUNK_SIZE_IRRLICHT = 2400;

class TerrainChunk {
public:
    TerrainChunk(irr::scene::ITerrainSceneNode* _terrain);
    TerrainChunk(const TerrainChunk& other) = default;
    TerrainChunk(TerrainChunk&& other) = default;
    virtual ~TerrainChunk() = default;

    TerrainChunk& operator=(const TerrainChunk& other) = default;
    TerrainChunk& operator=(TerrainChunk&& other) = default;

    irr::scene::ITerrainSceneNode* sceneNode() const;

private:
    irr::scene::ITerrainSceneNode* terrain;
};

class TerrainManager {
public:
    using offset_t = int64_t;
    TerrainManager() = default;
    TerrainManager(TerrainManager&& other) = default;
    virtual ~TerrainManager() = default;

    TerrainManager& operator=(TerrainManager&& other) = default;

    const TerrainChunk& getChunk(offset_t off_x, offset_t off_y) const;
    TerrainChunk& getMutableChunk(offset_t off_x, offset_t off_y);
    void addChunk(offset_t off_x, offset_t off_y, const TerrainChunk& chunk);
    void addChunk(offset_t off_x, offset_t off_y, TerrainChunk&& chunk);
    void deleteChunk(offset_t off_x, offset_t off_y);

private:
    // XXX: maybe replace with unordered_map, but then wee need to write hash() function for
    // std::pair, which may not be a trivial task
    std::map<std::pair<offset_t, offset_t>, TerrainChunk> chunks;
};

extern TerrainManager terrainManager;

#endif /* end of include guard: WORLD_TERRAIN_HPP */
