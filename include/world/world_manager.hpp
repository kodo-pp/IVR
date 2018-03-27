#ifndef WORLD_WORLD_MANAGER_HPP
#define WORLD_WORLD_MANAGER_HPP

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
    // TODO: change std::vector to std::map <WorldId, World>
    WorldManager(std::vector <World> _worlds);

    WorldId getWorldId(std::wstring worldName);
    std::vector <World> & getWorldList();
    World& getWorld(WorldId id);

    void deleteWorld(WorldId id);
    void addWorld(WorldId world);

protected:
    std::vector <World> worlds;
};

extern WorldManager worldManager;

#endif /* end of include guard: WORLD_WORLD_MANAGER_HPP */
