#include <iostream>
#include <graphics/graphics.hpp>
#include <core/core.hpp>

// [no utf-8]
void init(std::vector <std::string> * args) {
    std::setlocale(LC_CTYPE, "");
    std::wcerr << std::boolalpha;
    std::wcout << std::boolalpha;

    initilaizeCore(args);
    initializeGraphics(args);
}
