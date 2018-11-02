#include <iostream>

#include <modbox/core/core.hpp>
#include <modbox/core/destroy.hpp>
#include <modbox/core/memory_manager.hpp>
#include <modbox/game/enemy.hpp>
#include <modbox/graphics/graphics.hpp>
#include <modbox/log/log.hpp>
#include <modbox/net/net.hpp>

#include <signal.h>
#include <unistd.h>

// [no utf-8]
void init(std::vector<std::string>& args)
{
    std::setlocale(LC_CTYPE, "");
    std::wcerr << std::boolalpha;
    std::wcout << std::boolalpha;

    std::setlocale(LC_NUMERIC, "C"); // Force std::to_string to use '.' as decimal point

    initilaizeCore(args);
    initializeGraphics(args);
    initializeEnemies();
    initializeGameObjects();

    signal(SIGINT, sigIntHandler);
    signal(SIGABRT, sigAbrtHandler);
    signal(SIGSEGV, sigSegvHandler);
}
