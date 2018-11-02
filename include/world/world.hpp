#ifndef WORLD_WORLD_HPP
#define WORLD_WORLD_HPP

#include <map>
#include <set>
#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <fstream>

#include <modbox/geometry/game_position.hpp>
#include <modbox/world/chunk.hpp>

#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

void createWorld(const std::string& name, const std::vector <std::string>& modules, bool buildMode);

#endif /* end of include guard: WORLD_WORLD_HPP */
