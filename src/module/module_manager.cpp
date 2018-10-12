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

void ModuleManager::loadModule(const std::string& moduleName, const std::vector<std::string>& args)
{
    // Validate module name
    if (!std::regex_match(moduleName, std::regex("[a-zA-Z0-9][a-zA-Z0-9_-]*"))) {
        throw std::runtime_error("Invalid module name: " + moduleName);
    }
    if (access(("modules/virtual/" + moduleName + "/module.so").c_str(), R_OK)) {
        loadVirtualModule(moduleName, args);
    } else if (access(("modules/plain/" + moduleName + "/start").c_str(), R_OK | X_OK)) {
        loadPlainModule(moduleName, args);
    } else {
        throw std::runtime_error("No such module: " + moduleName);
    }
}

void ModuleManager::loadPlainModule(const std::string& moduleName, std::vector<std::string> args)
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
        std::vector<char*> raw_argv;
        raw_argv.reserve(args.size() + 1);
        for (const std::string& arg : args) {
            // Надеюсь, оно не упадёт из-за этого
            raw_argv.emplace_back(const_cast<char*>(arg.c_str()));
        }
        raw_argv.emplace_back(nullptr);

        // Child process
        execv(modulePath.c_str(), raw_argv.data());
    } else {
        // Parent process
        LOG("Child process [" << pid << "] created successfully");
    }
}

void ModuleManager::loadVirtualModule(const std::string& moduleName,
                                      const std::vector<std::string>& args)
{
    LOG("Loading virtual module: " << moduleName);
    vmodManager.loadModule(moduleName, args);
}

ModuleManager moduleManager;
