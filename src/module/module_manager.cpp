#if false

#include <string>
#include <unordered_map>

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

    ModuleId addModule(int sock);
    void deleteModule(ModuleId id);

    void loadModule(ModuleId id);
    void unloadModule(ModuleId id);

    void sendModuleMessage(ModuleId moduleId, ModuleMessage message);
    ModuleMessage receiveModuleMessage(ModuleId moduleId, ModuleMessage message);

protected:
    std::unordered_map <ModuleId, Module> modules;
};

#endif
