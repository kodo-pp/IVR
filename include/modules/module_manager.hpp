#ifndef MODULES_MODULE_MANAGER_HPP
#define MODULES_MODULE_MANAGER_HPP

#include <functional>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
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

    void registerModule(const Module& module);
    void unregisterModule(const std::string& moduleName);
    void loadModule(const std::string& moduleName, const std::vector<std::string>& args = {});

protected:
    void loadPlainModule(const std::string& moduleName, std::vector<std::string> args);
    void loadVirtualModule(const std::string& moduleName, const std::vector<std::string>& args);
    std::unordered_map<std::string, Module> modules;
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

    std::function<FuncResult(const std::vector<std::string>&)> registerModuleFuncProvider(
            const std::string& name,
            std::string argTypes,
            std::string retTypes);
    std::vector<std::string> runModuleFunc(const std::string& id,
                                           const std::string& argTypes,
                                           const std::string& retTypes,
                                           const std::vector<std::string> arguments);

private:
    mutable std::mutex reverseMutex;
    mutable std::recursive_mutex mainMutex;
    std::unordered_set<std::string> moduleFuncs;
    Module module;
};

extern ModuleManager moduleManager;

#endif /* end of include guard: MODULES_MODULE_MANAGER_HPP */
