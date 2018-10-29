#include <regex>
#include <unordered_map>

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

void ModuleManager::loadModule(const std::string& moduleName, const std::vector<std::string>& _args)
{
    auto args = _args; // Copy _args
    LOG("Loading plain module: " << moduleName);
    // TODO: add manifest file or something like that
    std::string modulePath = "modules/mod_" + moduleName + "/module";

    // Check if the module exists
    if (access(modulePath.c_str(), R_OK | X_OK) < 0) {
        LOG("Access: " << modulePath);
        LOG("Module not found: '" << moduleName
                                  << "': file not found or is not executable: " << strerror(errno));
        throw std::runtime_error("Module not found: '" + moduleName
                                 + "': file not found or is not executable");
    }
    std::vector<const char*> argv{"start", nullptr};

    LOG("Forking...");
    auto pid = fork();
    if (pid == -1) {
        LOG("fork() failed: " << strerror(errno));
        throw std::runtime_error("fork() failed: " + std::string(strerror(errno)));
    } else if (pid == 0) {
        std::vector<char*> raw_argv;
        raw_argv.reserve(args.size() + 4);

        raw_argv.emplace_back(const_cast<char*>(modulePath.c_str()));
        raw_argv.emplace_back(const_cast<char*>("--main-port=44145"));
        raw_argv.emplace_back(const_cast<char*>("--reverse-port=54144"));
        for (const std::string& arg : args) {
            // Надеюсь, оно не упадёт из-за этого
            raw_argv.emplace_back(const_cast<char*>(arg.c_str()));
        }
        raw_argv.emplace_back(nullptr);

        // Child process
        execv(modulePath.c_str(), raw_argv.data());

        // If execv() failed, then exit unsuccessfully

        // Do not uncomment.
        // Causes severe (not destructive though) race condition between processes
        // LOG("execv() failed");
        exit(1);
    } else {
        // Parent process
        LOG("Child process [" << pid << "] created successfully");
    }
}

ModuleManager moduleManager;
