#ifndef WORLD_WORLD_HPP
#define WORLD_WORLD_HPP

#include <cstring>
#include <exception>
#include <fstream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include <modbox/geometry/game_position.hpp>
#include <modbox/world/chunk.hpp>

#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void createWorld(const std::string& name, const std::vector<std::string>& modules, bool buildMode);
std::vector<std::string> listWorlds();
void enterWorld(const std::string& name);

std::string getSavePath();

#endif /* end of include guard: WORLD_WORLD_HPP */
