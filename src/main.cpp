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
#include <core/memory_manager.hpp>
#include <graphics/graphics.hpp>
#include <log/log.hpp>

struct FuncResult * testFunc(const std::vector <void *> & args) {
    assert(args.size() >= 1);
    return (struct FuncResult *)args[0];
}

int main(int argc, char** argv) {
    std::setlocale(LC_CTYPE, "");
    std::wcerr << std::boolalpha;
    std::wcout << std::boolalpha;
    // [no utf-8]
    std::vector <std::string> * args = new std::vector <std::string> (argc);
    for (int i = 0; i < argc; ++i) {
        args->at(i) = argv[i];
    }

    getLogStream() << L"test 1" << lssNewline;
    sleep(2);
    getLogStream() << L"test 2\n";
    sleep(2);
    getLogStream() << lssBeginLine << L"test finished" << lssNewline;

    log(L"ModBox version " << _PROJECT_VERSION);

    log(L"Тест менеджера памяти");
    //log("a) ")
    void * p1 = malloc(100);

    log(memoryManager.track(p1));
    log(memoryManager.isTracking(p1));
    log(memoryManager.track(p1));
    log(memoryManager.isTracking(p1));
    log(memoryManager.forget(p1));
    log(memoryManager.isTracking(p1));
    log(memoryManager.forget(p1));
    log(memoryManager.isTracking(p1));
    log(memoryManager.track(p1));
    log(memoryManager.isTracking(p1));
    log(memoryManager.freePtr(p1));
    log(memoryManager.isTracking(p1));
    log(L"<Должен быть Segmentation Fault (Оказывается, не должен)> ");
    int * ip1 = (int *)p1;
    int b = *ip1; // Должен быть Segfault (Оказывается, не должен)
    log(L"<Но будет>");
    kill(getpid(), SIGSEGV);
    log(L"Тест завершён"); // Этого не должно случиться


    initilaizeCore(args);
    initializeGraphics(args);
    delete args;

    FuncProvider* prov = new FuncProvider(L"testCommand", testFunc);
    bool success = registerFuncProvider(prov);
    if (!success) {
        return 1;
    }
    std::vector <void *> callArgs;
    callArgs.push_back((void *)1234567);
    FuncProvider* remoteProv = getFuncProvider(L"testCommand");
    assert(remoteProv != nullptr);
    struct FuncResult * retval = (*remoteProv)(callArgs);
    assert((void *)retval == (void *)1234567);

    // TODO: разрегистрировать перед удалением
    delete prov;

    return 0;
}
