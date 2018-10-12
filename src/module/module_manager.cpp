#include <unordered_map>

#include <modbox/core/virtual_module.hpp>
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

void ModuleManager::registerModule(const Module& module)
{
    if (modules.count(module.getName()) > 0) {
        throw std::runtime_error("Tried to register already registered module '" + module.getName()
                                 + "'");
    }
    modules.insert({module.getName(), module});
}
void ModuleManager::unregisterModule(const std::string& moduleName)
{
    if (modules.count(moduleName) == 0) {
        throw std::runtime_error("Tried to unregister not registered module '" + moduleName + "'");
    }
    modules.erase(moduleName);
}

void ModuleManager::loadModule(const std::string& moduleName, const std::vector<std::string>& args)
{
    try {
        vmodManager.loadModule(moduleName)
    }
}

ModuleManager moduleManager;
