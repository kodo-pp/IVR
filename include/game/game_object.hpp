#ifndef GAME_GAME_OBJECT_HPP
#define GAME_GAME_OBJECT_HPP

#include <string>

#include <modbox/geometry/game_position.hpp>
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <map>

#include <modbox/modules/module.hpp>

#include <irrlicht_wrapper.hpp>

using GameObjectId = uint64_t;

class GameObject
{
public:
    GameObject(irr::scene::ISceneNode* _node, const std::string& _kind, GameObjectId _id);
    GameObject(const GameObject& other);
    GameObject(GameObject&& other);
    virtual ~GameObject();

    GameObject& operator=(const GameObject& other);
    GameObject& operator=(GameObject&& other);

    std::string getKind() const;

    GamePosition getPosition() const;
    void setPosition(const GamePosition& newPosition);
    bool attachPart(const std::string& partKind);
    irr::scene::ISceneNode* sceneNode() const;

protected:
    GameObjectId id;
    std::string kind;
    irr::scene::ISceneNode* node;
    irr::scene::ITriangleSelector* selector;
};

class GameObjectManager
{
public:
    GameObjectManager() = default;
    GameObjectManager(const GameObjectManager& other) = default;
    GameObjectManager(GameObjectManager&& other) = default;
    virtual ~GameObjectManager() = default;

    GameObjectManager& operator=(const GameObjectManager& other) = default;
    GameObjectManager& operator=(GameObjectManager&& other) = default;

    GameObjectId createGameObject(const std::string& kind, irr::scene::ISceneNode* model);
    void deleteGameObject(GameObjectId id);
    const GameObject& access(GameObjectId id);
    GameObject& mutableAccess(GameObjectId id);
    std::optional<GameObjectId> reverseLookup(irr::scene::ISceneNode* drawable);

    void addKind(const std::string& kind);
    void addRecipe(const std::string& kind, const std::string& partKind, const std::string& resultingKind);
    std::optional <std::string> checkRecipe(const std::string& kind, const std::string& partKind);

private:
    mutable std::recursive_mutex mutex;
    std::unordered_set<std::string> kinds;
    std::map<std::pair<std::string, std::string>, std::string> recipes;
    std::unordered_map<GameObjectId, GameObject> gameObjects;
    GameObjectId idCounter = 0;
};

GameObjectManager& getGameObjectManager();

void initializeGameObjects();

#endif /* end of include guard: GAME_GAME_OBJECT_HPP */
