#ifndef WORLD_TERRAIN_HPP
#define WORLD_TERRAIN_HPP

#include <irrlicht.h>
#include <string>

class TerrainChunk {
public:
    using offset_t = int64_t;

    TerrainChunk(offset_t off_x, offset_t off_y, const std::wstring& heightmap,
                 irr::video::ITexture* tex, irr::video::ITexture* detail);
    TerrainChunk(const TerrainChunk& other) = default;
    TerrainChunk(TerrainChunk&& other) = default;
    virtual ~TerrainChunk() = default;

    TerrainChunk& operator=(const TerrainChunk& other) = default;
    TerrainChunk& operator=(TerrainChunk&& other) = default;

    irr::scene::ITerrainSceneNode* sceneNode();

private:
    irr::scene::ITerrainSceneNode* node;
};

#endif /* end of include guard: WORLD_TERRAIN_HPP */
