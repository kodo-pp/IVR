#ifndef GAME_OBJECTS_CUBE_HPP
#define GAME_OBJECTS_CUBE_HPP

#include <modbox/game/game_object.hpp>

class GameObjCube : public GameObject
{
public:
    GameObjCube(ISceneNode*);
    GameObjCube(std::wstring) = delete;
};

#endif /* end of include guard: GAME_OBJECTS_CUBE_HPP */
