#include <core/core.hpp>
#include <core/init.hpp>
#include <core/memory_manager.hpp>
#include <graphics/graphics.hpp>
#include <log/log.hpp>
#include <net/net.hpp>
#include <thread>
#include <util/util.hpp>

std::atomic<bool> doWeNeedToShutDown(false);

void destroy(void) {
    LOG(L"I will kill... client threads!");
    joinModuleListenerThread();
    LOG(L"No more functions");
    funcProvidersCleanup();
    LOG(L"And graphics");
    cleanupGraphics();
    LOG(L"I'll forget anything!");
    memoryManager.freeAll();
    LOG(L"I'm dying...");
    exit(0);
}

void sigIntHandler(UNUSED int signal) {
    LOG(L"Wow, you interrupted me! How rude...");
    doWeNeedToShutDown = true;
}
