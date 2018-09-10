#include <iostream>

#include <core/core.hpp>
#include <core/destroy.hpp>
#include <core/memory_manager.hpp>
#include <game/enemy.hpp>
#include <graphics/graphics.hpp>
#include <log/log.hpp>
#include <net/net.hpp>

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

    signal(SIGINT, sigIntHandler);
}
