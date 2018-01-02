#include <core/core.hpp>
#include <vector>
#include <string>
#include <iostream>

int main(int argc, char** argv) {
    std::vector <std::string> * args = new std::vector< std::string > (argc);
    for (int i = 0; i < argc; ++i) {
        args->at(i) = argv[i];
    }

    std::cout << "Initializing core" << std::endl;
    std::cout.flush();
    bool _ = initilaizeCore(args);
    std::cout << "Initialized core" << std::endl;
    std::cout.flush();
    std::cout << "Exiting" << std::endl;
    delete args;
    return 0;
}
