#ifndef MODULES_MODULE_MANAGER_HPP
#define MODULES_MODULE_MANAGER_HPP

#include <modules/module.hpp>
#include <string>
#include <unordered_map>

// TEMP: maybe we should change it to something more complex
using ModuleMessage = std::wstring;

/**
 * Manages modules
 *
 * There should be only one instance of it
 */

class ModuleManager {
public:
    ModuleManager(std::unordered_map<ModuleId, Module> _modules);

    ModuleId addModule(int sock);
    void deleteModule(ModuleId id);

    void loadModule(ModuleId id);
    void unloadModule(ModuleId id);

    void sendModuleMessage(ModuleId moduleId, ModuleMessage message);
    ModuleMessage receiveModuleMessage(ModuleId moduleId, ModuleMessage message);

protected:
    std::unordered_map<ModuleId, Module> modules;
};

class ModuleWorker {
public:
    ModuleWorker(int _sock);
    ~ModuleWorker();

    void work();
    void please_work() noexcept;

private:
    int sock;
};

extern ModuleManager moduleManager;

#endif /* end of include guard: MODULES_MODULE_MANAGER_HPP */
