#include <sstream>
#include <string>

#include <graphics/graphics.hpp>
#include <log/log.hpp>
#include <world/terrain.hpp>

#include <irrlicht.h>

void TerrainManager::loadTerrain(offset_t off_x, offset_t off_y)
{
    LOG("TerrainManager @" << this << ": loading terrain at " << off_x << ", " << off_y);
    // STUB
    if (off_x == 0 && off_y == 0) {
        graphicsLoadTerrain(0,
                            0,
                            L"textures/terrain/heightmap/heightmap1.png",
                            graphicsLoadTexture(L"textures/terrain/clouds.png"),
                            graphicsLoadTexture(L"textures/terrain/details1.png"));
    } else if (off_x == 1 && off_y == 0) {
        graphicsLoadTerrain(1,
                            0,
                            L"textures/terrain/heightmap/heightmap2.png",
                            graphicsLoadTexture(L"textures/terrain/clouds.png"),
                            graphicsLoadTexture(L"textures/terrain/details2.png"));
    } else {
        throw std::runtime_error("Invalid chunk position");
    }
    graphicsHandleCollisions(getChunk(off_x, off_y).sceneNode());
}

const Chunk& TerrainManager::getChunk(offset_t off_x, offset_t off_y) const
{
    LOG("TerrainManager @" << this << ": getting chunk at " << off_x << ", " << off_y);
    return chunks.at({off_x, off_y});
}
Chunk& TerrainManager::getMutableChunk(offset_t off_x, offset_t off_y)
{
    return chunks.at({off_x, off_y});
}
void TerrainManager::addChunk(offset_t off_x, offset_t off_y, const Chunk& chunk)
{
    LOG("TerrainManager @" << this << ": adding chunk at " << off_x << ", " << off_y);
    bool newInserted = false;
    std::tie(std::ignore, newInserted) = chunks.insert({{off_x, off_y}, chunk});
    if (!newInserted) {
        std::stringstream ss;
        ss << "attempted to double-add chunk (" << off_x << ", " << off_y << ")";
        throw std::runtime_error(ss.str());
    }
}
void TerrainManager::addChunk(offset_t off_x, offset_t off_y, Chunk&& chunk)
{
    LOG("TerrainManager @" << this << ": adding chunk&& at " << off_x << ", " << off_y);
    bool newInserted = false;
    std::tie(std::ignore, newInserted) = chunks.insert({{off_x, off_y}, chunk});
    if (!newInserted) {
        std::stringstream ss;
        ss << "attempted to double-add chunk (" << off_x << ", " << off_y << ")";
        throw std::runtime_error(ss.str());
    }
}

void TerrainManager::deleteChunk(offset_t off_x, offset_t off_y)
{
    LOG("TerrainManager @" << this << ": deleting chunk at " << off_x << ", " << off_y);
    chunks.erase({off_x, off_y});
}

void TerrainManager::trackMob(EnemyId mobId)
{
    auto chunk = enemyManager.accessEnemy(mobId).getPosition().getChunk();
    enemies.insert({mobId, chunk});
    chunks.at(chunk).trackMob(mobId);
}
void TerrainManager::updateMob(EnemyId mobId)
{
    GamePosition pos = enemyManager.accessEnemy(mobId).getPosition();
    auto realChunkPos = pos.getChunk();
    auto currentChunkPos = enemies.at(mobId);
    if (realChunkPos != currentChunkPos) {
        chunks.at(currentChunkPos).forgetMob(mobId);
        chunks.at(realChunkPos).trackMob(mobId);
    }
}
void TerrainManager::forgetMob(EnemyId mobId)
{
    auto chunkPos = enemies.at(mobId);
    chunks.at(chunkPos).forgetMob(mobId);
}

void TerrainManager::mobsAi()
{
    for (auto& kv : chunks) {
        Chunk& v = kv.second;
        v.mobsAi();
    }
}

void trackObject(GameObjectId objectId);
void updateObject(GameObjectId objectId);
void forgetObject(GameObjectId objectId);

TerrainManager terrainManager;
