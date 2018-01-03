#include <core/core.hpp>
#include <graphics/graphics.hpp>
#include <functional>
#include <vector>
#include <string>
#include <iostream>
#include <cassert>

struct FuncResult * testFunc(const std::vector <void *> & args) {
    assert(args.size() >= 1);
    std::cout << "Test!!!" << std::endl;
    return (struct FuncResult *)args[0];
}

int main(int argc, char** argv) {
    std::vector <std::string> * args = new std::vector< std::string > (argc);
    for (int i = 0; i < argc; ++i) {
        args->at(i) = argv[i];
    }

    std::cout << "Initializing core" << std::endl;
    std::cout.flush();
    initilaizeCore(args);
    initializeGraphics(args);
    std::cout << "Initialized core" << std::endl;
    std::cout.flush();
    delete args;

    std::cout << "Registering some FuncProvider" << std::endl;
    FuncProvider* prov = new FuncProvider("testCommand", testFunc);
    bool success = registerFuncProvider(prov);
    if (!success) {
        std::cout << "Failed" << std::endl;
        return 1;
    }
    std::cout << "Success" << std::endl;

    std::cout << "Calling command 'testCommand'" << std::endl;
    std::vector <void *> callArgs;
    callArgs.push_back((void *)1234567);
    FuncProvider* remoteProv = getFuncProvider("testCommand");
    assert(remoteProv != nullptr);
    struct FuncResult * retval = (*remoteProv)(callArgs);
    std::cout << "retval = " << (size_t)(void *)retval << std::endl;
    assert((void *)retval == (void *)1234567);
    std::cout << "OK" << std::endl;

    // TODO: разрегистрировать перед удалением
    delete prov;

    std::cout << "Exiting" << std::endl;
    return 0;
}
