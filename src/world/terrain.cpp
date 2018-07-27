#include <irrlicht.h>
#include <string>

#include <world/terrain.hpp>

TerrainChunk::TerrainChunk(offset_t off_x, offset_t off_y, const std::wstring& heightmap,
                           irr::video::ITexture* tex, irr::video::ITexture* detail) {}
irr::scene::ITerrainSceneNode* sceneNode();
