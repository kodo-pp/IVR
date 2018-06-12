#include <iostream>
#include <graphics/graphics.hpp>
#include <core/core.hpp>
#include <net/net.hpp>
#include <unistd.h>
#include <signal.h>
#include <log/log.hpp>
#include <core/memory_manager.hpp>
#include <core/destroy.hpp>

struct FuncResult* handlerTest(std::vector <void*> args) {
    int a = *(int32_t*)args.at(0);
    int b = *(int32_t*)args.at(1);
    std::string s1 = *(std::string*)args.at(2);
    std::string s2 = *(std::string*)args.at(3);

    int64_t c = (int64_t)a + b;
    std::string z = s1 + s2;

    auto ret = new struct FuncResult;
    int64_t* cp = new int64_t(c);
    std::string* zp = new std::string(z);

    ret->data.push_back(cp);
    ret->data.push_back(zp);
    ret->exitStatus = 0;
    return ret;
}

// [no utf-8]
void init(std::vector <std::string> * args) {
    std::setlocale(LC_CTYPE, "");
    std::wcerr << std::boolalpha;
    std::wcout << std::boolalpha;

    std::setlocale(LC_NUMERIC, "C"); // Force std::to_string to use '.' as decimal point

    initilaizeCore(args);
    initializeGraphics(args);

    registerFuncProvider(new FuncProvider("test", handlerTest), "iiss", "ls");

    signal(SIGINT, sigIntHandler);

}
