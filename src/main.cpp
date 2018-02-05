#include <core/core.hpp>
#include <graphics/graphics.hpp>
#include <functional>
#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <log/log.hpp>
#include <core/memory_manager.hpp>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

struct FuncResult * testFunc(const std::vector <void *> & args) {
    assert(args.size() >= 1);
    return (struct FuncResult *)args[0];
}

int main(int argc, char** argv) {
    std::vector <std::string> * args = new std::vector< std::string > (argc);
    for (int i = 0; i < argc; ++i) {
        args->at(i) = argv[i];
    }
    getLogStream() << "test 1" << lssNewline;
    sleep(2);
    getLogStream() << "test 2\n";
    sleep(2);
    getLogStream() << "test finished" << lssNewline;



    //getLogStream() << "Тест менеджера памяти" << lssNewline;
    //getLogStream() <<  << lssNewLine;
    log("Тест менеджера памяти");
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
    log("<Должен бqыть Segmentation Fault (Оказывается, не должен)> ");
    int * ip1 = (int *)p1;
    int b = *ip1; // Должен быть Segfault (Оказывается, не должен)
    log("<Но будет>");
    kill(getpid(), SIGSEGV);
    log("Тест завершён"); // Этого не должно случиться


    initilaizeCore(args);
    initializeGraphics(args);
    delete args;

    FuncProvider* prov = new FuncProvider("testCommand", testFunc);
    bool success = registerFuncProvider(prov);
    if (!success) {
        return 1;
    }
    std::vector <void *> callArgs;
    callArgs.push_back((void *)1234567);
    FuncProvider* remoteProv = getFuncProvider("testCommand");
    assert(remoteProv != nullptr);
    struct FuncResult * retval = (*remoteProv)(callArgs);
    assert((void *)retval == (void *)1234567);

    // TODO: разрегистрировать перед удалением
    delete prov;

    return 0;
}
