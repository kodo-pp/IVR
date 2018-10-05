#ifndef GAME_AI_HPP
#define GAME_AI_HPP

#include <functional>

#include <modbox/geometry/game_position.hpp>
std::function<GamePosition()> getDefaultAiFunc();

#endif /* end of include guard: GAME_AI_HPP */
