#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <core/core.hpp>
#include <core/destroy.hpp>
#include <core/init.hpp>
#include <core/memory_manager.hpp>
#include <game/game_loop.hpp>
#include <graphics/graphics.hpp>
#include <log/log.hpp>
#include <net/net.hpp>
#include <util/handle_storage.hpp>
#include <util/testit.hpp>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

struct FuncResult* testFunc(const std::vector<void*>& args)
{
    assert(args.size() >= 1);
    return (struct FuncResult*)args[0];
}

int main(int argc, char** argv)
{
    // Test for HandleStorage. TODO: move in more convenient place
    {
        HandleStorage<int, int> hs;

        testit_begin("HandleStorage");
        testit(hs.insert(25) == 0);
        testit(hs.insert(44) == 1);
        testit(hs.insert(354) == 2);
        testit((hs.remove(1), true));
        testit(hs.insert(33) == 1);
        testit(hs.access(2) == 354);
        testit((hs.mutableAccess(1) = 66, true));
        testit(hs.access(1) == 66);
        testit_end();
    }

    // [no utf-8]
    std::vector<std::string>* args = new std::vector<std::string>(argc);
    for (int i = 0; i < argc; ++i) {
        args->at(i) = argv[i];
    }
    init(args);
    delete args;

    LOG(L"ModBox version " << _PROJECT_VERSION);
    createModuleListenerThread();
    gameLoop();
    destroy();
    return 0;
}
