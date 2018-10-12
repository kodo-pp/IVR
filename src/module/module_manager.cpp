#include <regex>
#include <unordered_map>

#include <modbox/core/virtual_module.hpp>
#include <modbox/modules/module_manager.hpp>

#include <spawn.h>
#include <unistd.h>

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

void ModuleManager::loadModule(const std::string& moduleName)
{
    // Validate module name
    if (!std::regex_match(moduleName, std::regex("[a-zA-Z0-9][a-zA-Z0-9_-]*"))) {
        throw std::runtime_error("Invalid module name: " + moduleName);
    }
    if (access(("modules/virtual/" + moduleName + "/start").c_str(), R_OK | X_OK)) {
        loadVirtualModule(moduleName);
    } else if (access(("modules/plain/" + moduleName + "/start").c_str(), R_OK | X_OK)) {
        loadPlainModule(moduleName);
    } else {
        throw std::runtime_error("No such module: " + moduleName);
    }
}

void ModuleManager::loadPlainModule(const std::string& moduleName)
{
    LOG("Loading plain module: " << moduleName);
    // TODO: add manifest file or something like that
    std::string modulePath = "modules/plain/" + moduleName + "/start";
    std::vector<const char*> argv{"start", nullptr};

    LOG("Forking...");
    auto pid = fork();
    if (pid == -1) {
        LOG("fork() failed: " << strerror(errno));
        throw std::runtime_error("fork() failed: " + std::string(strerror(errno)));
    } else if (pid == 0) {
        // Child process
        execl(modulePath.c_str(), "start", nullptr);
    } else {
        // Parent process
        LOG("Child process [" << pid << "] created successfully");
    }
}

void ModuleManager::loadVirtualModule(const std::string& moduleName)
{
    LOG("Loading virtual module: " << moduleName);
}

ModuleManager moduleManager;
