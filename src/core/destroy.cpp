#include <core/init.hpp>
#include <core/memory_manager.hpp>
#include <log/log.hpp>
#include <net/net.hpp>
#include <thread>

std::atomic <bool> doWeNeedToShutDown(false);

void destroy(void) {
    log(L"I will kill... client threads!");
    joinModuleListenerThread();
    log(L"I'll forget anything!");
    memoryManager.freeAll();
    log(L"I'm dying...");
    exit(0);
}

void sigIntHandler(int signal) {
    log(L"Wow, you interrupted me! How rude...");
    doWeNeedToShutDown = true;
}
