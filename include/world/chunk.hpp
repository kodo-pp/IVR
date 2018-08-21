#ifndef WORLD_CHUNK_HPP
#define WORLD_CHUNK_HPP

#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <game/enemy.hpp>
#include <game/game_object.hpp>
#include <geometry/game_position.hpp>

#include <irrlicht.h>

/**
 * Represents a chunk (a relatively small area in the world)
 *
 * Chunk has game objects inside, and they (game objects) can only be located
 * in chunks.
 */

class Chunk
{
public:
    explicit Chunk(std::unordered_map<GameObjectId, GameObject> _objects,
                   irr::scene::ITerrainSceneNode* _terrain);
    Chunk(const Chunk& other) = default;
    Chunk(Chunk&& other) = default;
    virtual ~Chunk() = default;

    Chunk& operator=(const Chunk& other) = default;
    Chunk& operator=(Chunk&& other) = default;

    irr::scene::ITerrainSceneNode* sceneNode() const;

    void trackMob(EnemyId mobId);
    void forgetMob(EnemyId mobId);

    void mobsAi();

    GameObjectId addObject(GameObject&& object);
    void removeObject(GameObjectId objectId);
    void moveObject(GameObjectId objectId, GameObject& target);
    const GameObject& getObject(GameObjectId objectId);

private:
    irr::scene::ITerrainSceneNode* terrain;
    std::unordered_map<GameObjectId, GameObject> objects;
    std::unordered_set<EnemyId> mobs;
};

#endif /* end of include guard: WORLD_CHUNK_HPP */
