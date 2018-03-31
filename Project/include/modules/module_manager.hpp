#ifndef MODULES_MODULE_MANAGER_HPP
#define MODULES_MODULE_MANAGER_HPP

#include <string>
#include <map>
#include <modules/module.hpp>

// TEMP: maybe we should change it to something more complex
using ModuleMessage = std::wstring;

/**
 * Manages modules
 *
 * There should be only one instance of it
 */

class ModuleManager {
public:
    ModuleManager(std::unordered_map <ModuleId, Module> _modules);
    void loadModule(ModuleId id);
    void unloadModule(ModuleId id);
    void deleteModule(ModuleId id);

    void sendModuleMessage(ModuleId moduleId, ModuleMessage message);
    ModuleMessage recieveModuleMessage(ModuleId moduleId, ModuleMessage message);

protected:
    std::unordered_map <ModuleId, Module> modules;
};

extern ModuleManager moduleManager;

#endif /* end of include guard: MODULES_MODULE_MANAGER_HPP */
