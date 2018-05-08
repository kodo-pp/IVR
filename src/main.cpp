#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <cassert>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <core/core.hpp>
#include <core/init.hpp>
#include <core/memory_manager.hpp>
#include <graphics/graphics.hpp>
#include <log/log.hpp>
#include <game/game_loop.hpp>
#include <net/net.hpp>
#include <core/destroy.hpp>

struct FuncResult * testFunc(const std::vector <void *> & args) {
    assert(args.size() >= 1);
    return (struct FuncResult *)args[0];
}

int main(int argc, char** argv) {
    // [no utf-8]
    std::vector <std::string> * args = new std::vector <std::string> (argc);
    for (int i = 0; i < argc; ++i) {
        args->at(i) = argv[i];
    }
    init(args);
    delete args;

    log(L"ModBox version " << _PROJECT_VERSION);

    createModuleListenerThread();

    gameLoop();

    destroy();

    return 0;
}
