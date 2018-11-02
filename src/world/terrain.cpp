#include <sstream>
#include <string>

#include <modbox/graphics/graphics.hpp>
#include <modbox/log/log.hpp>
#include <modbox/world/terrain.hpp>
#include <modbox/world/world.hpp>

#include <irrlicht_wrapper.hpp>
#include <sys/stat.h>

bool TerrainManager::hasGeneratedTerrain(offset_t off_x, offset_t off_y)
{
    return access(getTerrainFilename(off_x, off_y).c_str(), R_OK) == 0;
}

void TerrainManager::loadTerrain(offset_t off_x, offset_t off_y)
{
    if (!hasGeneratedTerrain(off_x, off_y)) {
        generateTerrain(off_x, off_y);
    } else {
        LOG("Loading terrain at (" << off_x << ", " << off_y << ")");
        graphicsLoadTerrain(off_x,
                            off_y,
                            getTerrainFilename(off_x, off_y),
                            nullptr,
                            graphicsLoadTexture("textures/terrain/details1.png"));
        drawBarrier();
        graphicsHandleCollisions(getChunk(off_x, off_y).sceneNode());
    }
    maybeUpdateJunctions(off_x - 1, off_y);
    maybeUpdateJunctions(off_x, off_y - 1);
}

void TerrainManager::maybeUpdateJunctions(UNUSED offset_t x, UNUSED offset_t y)
{
    LOG("maybeUpdateJunctions: skipping as it does not work");
    return;
    // LOG("maybeUpdateJunctions(" << x << ", " << y << ")");
    // if (!hasChunk(x, y)) {
    //     LOG("  maybeUpdateJunctions: !hasChunk(x, y)");
    //     return;
    // }
    //
    // if (hasChunk(x + 1, y)) {
    //     LOG("  maybeUpdateJunctions: hasChunk(x + 1, y)");
    //
    //     auto otherChunk = getChunk(x + 1, y);
    //
    //     std::vector<int> values;
    //     values.reserve(CHUNK_SIZE);
    //
    //     graphicsVisitTerrain(
    //             otherChunk.sceneNode(), 0, 0, CHUNK_SIZE, 1, [&](int /* x */, int /* y */, int v)
    //             {
    //                 values.push_back(v);
    //             });
    //
    //     graphicsModifyTerrain(
    //             getMutableChunk(x, y).sceneNode(),
    //             0,
    //             CHUNK_SIZE - 1,
    //             CHUNK_SIZE,
    //             CHUNK_SIZE,
    //             [&](int dx, int /* dy */, int /* v */) -> int { return values.at(dx); });
    // }
    //
    // if (hasChunk(x, y + 1)) {
    //     LOG("  maybeUpdateJunctions: hasChunk(x, y + 1)");
    //     auto otherChunk = getChunk(x, y + 1);
    //
    //     std::vector<int> values;
    //     values.reserve(CHUNK_SIZE);
    //
    //     graphicsVisitTerrain(
    //             otherChunk.sceneNode(), 0, 0, 1, CHUNK_SIZE, [&](int /* x */, int /* y */, int v)
    //             {
    //                 values.push_back(v);
    //             });
    //
    //     graphicsModifyTerrain(getMutableChunk(x, y).sceneNode(),
    //                           CHUNK_SIZE - 1,
    //                           0,
    //                           CHUNK_SIZE,
    //                           CHUNK_SIZE,
    //                           [&](int dx, int dy, int /* v */) -> int {
    //                               LOG("lambda(dx=" << dx << ", dy=" << dy << ")");
    //                               // return values.at(dy);
    //                               return 255;
    //                           });
    // }
}

const Chunk& TerrainManager::getChunk(offset_t off_x, offset_t off_y) const
{
    LOG("TerrainManager @" << this << ": getting chunk at " << off_x << ", " << off_y);
    return chunks.at({off_x, off_y});
}
Chunk& TerrainManager::getMutableChunk(offset_t off_x, offset_t off_y)
{
    LOG("TerrainManager @" << this << ": getting chunk at " << off_x << ", " << off_y);
    try {
        return chunks.at({off_x, off_y});
    } catch (const std::out_of_range& e) {
        LOG("no such chunk");
        logStackTrace();
        std::rethrow_exception(std::current_exception());
    }
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
    // maybeUpdateJunctions(off_x - 1, off_y);
    // maybeUpdateJunctions(off_x, off_y - 1);
    // maybeUpdateJunctions(off_x + 1, off_y);
    // maybeUpdateJunctions(off_x, off_y + 1);
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
    getOrCreateChunk(chunk.first, chunk.second).trackMob(mobId);
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

bool TerrainManager::hasChunk(offset_t off_x, offset_t off_y)
{
    return chunks.count({off_x, off_y}) > 0;
}

void TerrainManager::mobsAi()
{
    for (auto& kv : chunks) {
        Chunk& v = kv.second;
        v.mobsAi();
    }
}

irr::video::IImage* defaultTerrainGenerator(UNUSED TerrainManager::offset_t xc,
                                            UNUSED TerrainManager::offset_t yc)
{
    auto image = getIrrlichtVideoDriver()->createImage(irr::video::ECF_R8G8B8,
                                                       {CHUNK_SIZE, CHUNK_SIZE});
    if (image == nullptr) {
        throw std::runtime_error("Unable to create heightmap image");
    }

    // auto xo = xc * CHUNK_SIZE;
    // auto yo = yc * CHUNK_SIZE;

    auto data = static_cast<uint8_t*>(image->lock());

    for (int64_t dx = 0; dx < CHUNK_SIZE; ++dx) {
        for (int64_t dy = 0; dy < CHUNK_SIZE; ++dy) {
            // auto x = xo + dx;
            // auto y = yo + dy;
            auto index = 3 * (dy * CHUNK_SIZE + dx);
            double value = sin(static_cast<double>(dx) / CHUNK_SIZE * 2 * M_PI)
                           + cos(static_cast<double>(dy) / CHUNK_SIZE * 2 * M_PI);
            const double maxValue = 2.0;
            const double minValue = -2.0;
            double normalizedValue = (value - minValue) / (maxValue - minValue);
            assert(index + 2 < 3 * CHUNK_SIZE * CHUNK_SIZE);

            // normalizedValue = (dx + dy) / 2.0 / CHUNK_SIZE;

            data[index] = static_cast<uint8_t>(normalizedValue * 256);
            data[index + 1] = static_cast<uint8_t>(normalizedValue * 256);
            data[index + 2] = static_cast<uint8_t>(normalizedValue * 256);
        }
    }
    image->unlock();

    return image;
}

TerrainManager::TerrainManager() : generator(defaultTerrainGenerator)
{
}

void TerrainManager::generateTerrain(offset_t x, offset_t y)
{
    LOG("Generating terrain at (" << x << ", " << y << ")");
    graphicsLoadTerrain(x,
                        y,
                        defaultTerrainGenerator(x, y),
                        nullptr,
                        graphicsLoadTexture("textures/terrain/details1.png"));
    drawBarrier();
    graphicsHandleCollisions(getChunk(x, y).sceneNode());
}

void TerrainManager::writeTerrain(offset_t x, offset_t y, irr::video::IImage* hm)
{
    getIrrlichtVideoDriver()->writeImageToFile(hm, getCreateTerrainFilename(x, y).c_str());
}

std::string TerrainManager::getTerrainFilename(offset_t x, offset_t y) const
{
    return getSavePath() + "terrain/heightmaps/" + std::to_string(x) + "_" + std::to_string(y)
           + ".png";
}

std::string TerrainManager::getCreateTerrainFilename(offset_t x, offset_t y)
{
    if (access((getSavePath() + "terrain").c_str(), R_OK | X_OK) < 0) {
        if (mkdir((getSavePath() + "terrain").c_str(), 0755) < 0) {
            throw std::runtime_error(std::string("Unable to create directory 'terrain': ")
                                     + strerror(errno));
        }
    }
    if (access((getSavePath() + "terrain/heightmaps").c_str(), R_OK | X_OK) < 0) {
        if (mkdir((getSavePath() + "terrain/heightmaps").c_str(), 0755) < 0) {
            throw std::runtime_error(
                    std::string("Unable to create directory 'terrain/heightmaps': ")
                    + strerror(errno));
        }
    }
    return getSavePath() + "terrain/heightmaps/" + std::to_string(x) + "_" + std::to_string(y)
           + ".png";
}

// void TerrainManager::trackObject(GameObjectId objectId);
// void TerrainManager::updateObject(GameObjectId objectId);
// void TerrainManager::forgetObject(GameObjectId objectId);

void TerrainManager::autoLoad(double px, double py)
{
    try {
        offset_t cx = floor(px / CHUNK_SIZE_IRRLICHT);
        offset_t cy = floor(py / CHUNK_SIZE_IRRLICHT);
        std::vector<std::pair<offset_t, offset_t>> chunklist;
        chunklist.emplace_back(cx, cy);
        chunklist.emplace_back(cx - 1, cy);
        chunklist.emplace_back(cx + 1, cy);
        chunklist.emplace_back(cx, cy - 1);
        chunklist.emplace_back(cx, cy + 1);
        chunklist.emplace_back(cx - 1, cy - 1);
        chunklist.emplace_back(cx + 1, cy - 1);
        chunklist.emplace_back(cx - 1, cy + 1);
        chunklist.emplace_back(cx + 1, cy + 1);
        setLoaded(chunklist);
    } catch (const std::exception& e) {
        LOG("Exception caught at TerrainManager::autoLoad(): " << e.what());
        std::rethrow_exception(std::current_exception());
    }
}

void TerrainManager::setLoaded(const std::vector<std::pair<offset_t, offset_t>> chunklist)
{
    for (auto [cx, cy] : chunklist) {
        if (!hasChunk(cx, cy)) {
            loadTerrain(cx, cy);
        }
    }
}

Chunk& TerrainManager::getOrCreateChunk(offset_t x, offset_t y)
{
    if (!hasChunk(x, y)) {
        loadTerrain(x, y);
    }
    return getMutableChunk(x, y);
}

TerrainManager terrainManager;
