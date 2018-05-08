#include <iostream>
#include <graphics/graphics.hpp>
#include <core/core.hpp>
#include <net/net.hpp>
#include <unistd.h>
#include <signal.h>
#include <log/log.hpp>
#include <core/memory_manager.hpp>
#include <core/destroy.hpp>

// [no utf-8]
void init(std::vector <std::string> * args) {
    std::setlocale(LC_CTYPE, "");
    std::wcerr << std::boolalpha;
    std::wcout << std::boolalpha;

    initilaizeCore(args);
    initializeGraphics(args);

    signal(SIGINT, sigIntHandler);
}
