#include <unordered_map>

#include <modbox/modules/module_manager.hpp>

static std::unordered_map<std::thread::id, ModuleWorker&> moduleWorkers;

ModuleWorker& ModuleManager::getModuleWorkerByThreadId(std::thread::id threadId)
{
    return moduleWorkers.at(threadId);
}

void ModuleManager::registerModuleWorker(ModuleWorker& worker, std::thread::id threadId)
{
    moduleWorkers.insert({threadId, worker});
}

ModuleManager moduleManager;
