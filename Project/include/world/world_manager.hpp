#ifndef WORLD_WORLD_MANAGER_HPP
#define WORLD_WORLD_MANAGER_HPP

#include <map>
#include <world/world.hpp>

// WARNING: in UML class diagram this class is named WorldsManager (with 's')
// Here it was renamed to WorldManager (w/o 's')

/**
 * Manages worlds
 *
 * There should be only one instance of it
 * NOTICE: (almost) all values are returned by reference
 */
class WorldManager {
public:
    WorldManager(std::unordered_map <WorldId, World> _worlds);

    WorldId getWorldId(std::wstring worldName);
    std::unordered_map <WorldId, World> & getWorldList();
    World& getWorld(WorldId id);

    void deleteWorld(WorldId id);
    void addWorld(WorldId world);

protected:
    std::unordered_map <WorldId, World> worlds;
};

extern WorldManager worldManager;

#endif /* end of include guard: WORLD_WORLD_MANAGER_HPP */
