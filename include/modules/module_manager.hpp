#ifndef MODULES_MODULE_MANAGER_HPP
#define MODULES_MODULE_MANAGER_HPP

#include <functional>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <modbox/core/core.hpp>
#include <modbox/modules/module.hpp>
#include <modbox/util/handle_storage.hpp>

// TEMP: maybe we should change it to something more complex
using ModuleMessage = std::wstring;

class ModuleWorker;

/**
 * Manages modules
 *
 * There should be only one instance of it
 */

class ModuleManager
{
public:
    // ModuleManager(std::unordered_map<ModuleId, ModuleWorker> _modules);
    ModuleManager() = default;

    // ModuleId addModule(int sock);
    // void deleteModule(ModuleId id);

    void registerModuleWorker(ModuleWorker& worker,
                              std::thread::id threadId = std::this_thread::get_id());
    ModuleWorker& getModuleWorkerByThreadId(std::thread::id threadId);

    // void loadModule(ModuleId id);
    // void unloadModule(ModuleId id);

    // void sendModuleMessage(ModuleId moduleId, ModuleMessage message);
    // ModuleMessage receiveModuleMessage(ModuleId moduleId, ModuleMessage message);

protected:
    // std::unordered_map<ModuleId, Module> modules;
};

class ModuleWorker
{
public:
    ModuleWorker(Module&& _module);
    ModuleWorker(const ModuleWorker& other) = delete;
    ModuleWorker(ModuleWorker&& other) = default;

    ModuleWorker& operator=(const ModuleWorker& other) = delete;
    ModuleWorker& operator=(ModuleWorker&& other) = default;
    ~ModuleWorker();

    void work();
    void please_work() noexcept;

    std::pair<uint64_t, std::function<FuncResult(const std::vector<std::string>&)>>
    registerModuleFuncProvider(const std::string& name, std::string argTypes, std::string retTypes);
    std::vector<std::string> runModuleFunc(uint64_t id,
                                           const std::string& argTypes,
                                           const std::string& retTypes,
                                           const std::vector<std::string> arguments);

private:
    mutable std::mutex reverseMutex;
    mutable std::recursive_mutex mainMutex;
    HandleStorage<uint64_t, std::string> moduleFuncs;
    Module module;
};

extern ModuleManager moduleManager;

#endif /* end of include guard: MODULES_MODULE_MANAGER_HPP */
