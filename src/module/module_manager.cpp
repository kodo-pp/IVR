#include <regex>
#include <unordered_map>

#include <modbox/modules/module_manager.hpp>

#include <boost/filesystem.hpp>
#include <spawn.h>
#include <unistd.h>

static std::unordered_map<std::thread::id, ModuleWorker&> moduleWorkers;

ModuleWorker& ModuleManager::getModuleWorkerByThreadId(std::thread::id threadId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return moduleWorkers.at(threadId);
}

void ModuleManager::registerModuleWorker(ModuleWorker& worker, std::thread::id threadId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    moduleWorkers.insert({threadId, worker});
}

void ModuleManager::registerModule(const Module& module)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    if (modules.count(module.getName()) > 0) {
        throw std::runtime_error("Tried to register already registered module '" + module.getName()
                                 + "'");
    }
    modules.insert({module.getName(), module});
}
void ModuleManager::unregisterModule(const std::string& moduleName)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    if (modules.count(moduleName) == 0) {
        throw std::runtime_error("Tried to unregister not registered module '" + moduleName + "'");
    }
    modules.erase(moduleName);
}

void ModuleManager::loadModule(const std::string& moduleName, const std::vector<std::string>& _args)
{
    do {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        auto args = _args; // Copy _args
        LOG("Loading plain module: " << moduleName);
        // TODO: add manifest file or something like that
        std::string modulePath = "modules/mod_" + moduleName + "/module";

        // Check if the module exists
        if (access(modulePath.c_str(), R_OK | X_OK) < 0) {
            LOG("Access: " << modulePath);
            LOG("Module not found: '"
                << moduleName << "': file not found or is not executable: " << strerror(errno));
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
            LOG("Child process [" << pid
                                  << "] created successfully. Waiting until module is ready...");
            break;
        }
    } while (false);
    // Так хитро, чтобы lock_guard в этой функции не мешал потом вызвать addReadyModule()
    while (!isReady(moduleName)) {
        std::this_thread::yield();
    }
    LOG("Module '" << moduleName << "' is ready");
}

bool ModuleManager::isReady(const std::string& moduleName) const
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return readyModules.count(moduleName) > 0;
}
void ModuleManager::addReadyModule(const std::string& moduleName)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    auto [_, ok] = readyModules.emplace(moduleName);
    std::ignore = _;
    if (!ok) {
        throw std::logic_error("Failed to add module to list of ready modules");
    }
}

ModuleManager moduleManager;

std::vector<std::string> listModules()
{
    if (access("modules/", R_OK | X_OK) != 0) {
        return {};
    }
    using namespace boost::filesystem;
    std::vector<std::string> result;
    for (auto& entry : directory_iterator(path("modules/"))) {
        if (access((entry.path().string() + "/module").c_str(), R_OK | X_OK) == 0) {
            auto fullPath = entry.path().string();
            if (fullPath.find("/mod_") == fullPath.npos) {
                continue;
            }
            std::string moduleName(fullPath.begin() + std::string("modules/mod_").size(),
                                   fullPath.end());
            result.push_back(moduleName);
        }
    }
    return result;
}
