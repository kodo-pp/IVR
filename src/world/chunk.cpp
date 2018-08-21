#include <world/chunk.hpp>

Chunk::Chunk(std::unordered_map<GameObjectId, GameObject> _objects,
             irr::scene::ITerrainSceneNode* _terrain) :
        terrain(_terrain),
        objects(_objects)
{
}

irr::scene::ITerrainSceneNode* Chunk::sceneNode() const
{
    return terrain;
}

void Chunk::trackMob(EnemyId mobId)
{
    mobs.insert(mobId);
}
void Chunk::forgetMob(EnemyId mobId)
{
    mobs.erase(mobId);
}

void Chunk::mobsAi()
{
    for (EnemyId mobId : mobs) {
        enemyManager.mutableAccessEnemy(mobId).ai();
    }
}
