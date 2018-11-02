#include <map>
#include <set>
#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <fstream>

#include <modbox/geometry/game_position.hpp>
#include <modbox/world/chunk.hpp>
#include <modbox/world/world.hpp>

#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
std::string escape(const std::string& name)
{
    std::string s;
    for (auto& i : name) {
        if (isalnum(i)) {
            s.push_back(i);
        } else {
            s.push_back('_');
        }
    }
    return s;
}

void createWorld(const std::string& name, const std::vector <std::string>& modules, bool buildMode)
{
    if (access("saves/", R_OK | X_OK) != 0) {
        if (mkdir("saves/", 0755) != 0) {
            throw std::runtime_error("Unable to create 'saves/': " + std::string(strerror(errno)));
        }
    }
    if (access(("saves/" + escape(name) + "/").c_str(), R_OK | X_OK) == 0) {
        throw std::runtime_error("World with this name already exists");
    }
    if (mkdir(("saves/" + escape(name) + "/").c_str(), 0755) != 0) {
        throw std::runtime_error("Unable to create 'saves/" + escape(name) + ")': " + std::string(strerror(errno)));
    }
    std::ofstream out("saves/" + escape(name) + "/world.txt");
    if (!out.is_open()) {
        throw std::runtime_error("Unable to open metadata file");
    }
    out << name << std::endl;
    out << modules.size() << std::endl;
    for (const auto& i : modules) {
        out << i << " ";
    }
    out << std::endl;
    out << (buildMode ? 1 : 0) << std::endl;
    out.close();
}
