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
#include <modbox/log/log.hpp>
#include <modbox/modules/module_manager.hpp>
#include <modbox/graphics/graphics.hpp>
#include <modbox/game/game_loop.hpp>

#include <boost/filesystem.hpp>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

static std::string savePath;

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
    std::vector <std::string> mods;
    for (auto& i : modules) {
        if (i != "") {
            mods.emplace_back(i);
        }
    }
    out << mods.size() << std::endl;
    for (const auto& i : mods) {
        out << i << " ";
    }
    out << std::endl;
    out << (buildMode ? 1 : 0) << std::endl;
    out.close();
}

std::vector <std::string> listWorlds()
{
    if (access("saves/", R_OK | X_OK) != 0) {
        return {};
    }
    using namespace boost::filesystem;
    std::vector <std::string> result;
    for (auto& entry : directory_iterator(path("saves/"))) {
        std::ifstream in(entry.path().string() + "/world.txt");
        if (!in.is_open()) {
            continue;
        }
        std::string name;
        std::getline(in, name);
        if (!in.good()) {
            continue;
        }
        result.push_back(name);
        in.close();
    }
    return result;
}

std::string findWorld(const std::string& target)
{
    if (access("saves/", R_OK | X_OK) != 0) {
        throw std::runtime_error("world not found");
    }
    using namespace boost::filesystem;
    for (auto& entry : directory_iterator(path("saves/"))) {
        std::ifstream in(entry.path().string() + "/world.txt");
        if (!in.is_open()) {
            continue;
        }
        std::string name;
        std::getline(in, name);
        if (!in.good()) {
            continue;
        }
        if (name == target) {
            return entry.path().string();
        }
    }
    throw std::runtime_error("world not found");
}

void enterWorld(const std::string& name)
{
    LOG("Entering world '" << name << "'");
    auto path = findWorld(name);
    savePath = path;
    std::ifstream worldInfo(path + "/world.txt");
    std::string _;
    std::getline(worldInfo, _);
    size_t size;
    worldInfo >> size;
    std::vector <std::string> modules(size);
    for (auto& i : modules) {
        worldInfo >> i;
    }
    int buildMode;
    worldInfo >> buildMode;
    worldInfo.close();
    for (auto& i : modules) {
        moduleManager.loadModule(i);
    }
    graphicsInitializeCollisions();
    gameLoop();

    sleep(1000000);
}

std::string getSavePath()
{
    return savePath + "/";
}
