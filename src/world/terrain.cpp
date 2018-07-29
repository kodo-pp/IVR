#include <irrlicht.h>
#include <sstream>
#include <string>

#include <world/terrain.hpp>

TerrainChunk::TerrainChunk(irr::scene::ITerrainSceneNode* _terrain) : terrain(_terrain) {}

irr::scene::ITerrainSceneNode* TerrainChunk::sceneNode() const {
    return terrain;
}

const TerrainChunk& TerrainManager::getChunk(offset_t off_x, offset_t off_y) const {
    return chunks.at({off_x, off_y});
}
TerrainChunk& TerrainManager::getMutableChunk(offset_t off_x, offset_t off_y) {
    return chunks.at({off_x, off_y});
}
void TerrainManager::addChunk(offset_t off_x, offset_t off_y, const TerrainChunk& chunk) {
    bool newInserted = false;
    std::tie(std::ignore, newInserted) = chunks.insert({{off_x, off_y}, chunk});
    if (!newInserted) {
        std::stringstream ss;
        ss << "attempted to double-add chunk (" << off_x << ", " << off_y << ")";
        throw std::runtime_error(ss.str());
    }
}
void TerrainManager::addChunk(offset_t off_x, offset_t off_y, TerrainChunk&& chunk) {
    bool newInserted = false;
    std::tie(std::ignore, newInserted) = chunks.insert({{off_x, off_y}, chunk});
    if (!newInserted) {
        std::stringstream ss;
        ss << "attempted to double-add chunk (" << off_x << ", " << off_y << ")";
        throw std::runtime_error(ss.str());
    }
}
void TerrainManager::deleteChunk(offset_t off_x, offset_t off_y) {
    chunks.erase({off_x, off_y});
}

TerrainManager terrainManager;
