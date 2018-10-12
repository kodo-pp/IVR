#include <thread>

#include <modbox/core/virtual_module.hpp>
#include <modbox/modules/module_manager.hpp>

VirtualModuleManager vmodManager;

VirtualModule::VirtualModule(const std::string& name)
        : dso(vmodManager.findModule(name)), module(-1, -1, name, {})
{
}

void VirtualModule::spawnModuleThread(const std::vector<std::string>& arguments)
{
    try {
        LOG("VirtualModule::spawnModuleThread");
        std::thread thr([this, arguments]() {
            dso.resolve<void, const std::vector<std::string>&>("module_main")(arguments);
            vmodManager.onModuleUnload(arguments.at(0));
        });
        thr.detach();
        LOG("VirtualModule::spawnModuleThread finished");
    } catch (const std::exception& e) {
        LOG("VirtualModule::spawnModuleThread: std::exception: " << wstring_cast(e.what()));
    }
}

const Module& VirtualModule::getModule() const
{
    return module;
}

void VirtualModuleManager::loadModule(const std::string& name, std::vector<std::string> args)
{
    args.insert(args.begin(), name);
    std::lock_guard<std::recursive_mutex> lock(mutex);
    VirtualModule m(findModule(name));
    moduleManager.registerModule(m.getModule());
    m.spawnModuleThread(args);
    modules.insert({name, std::move(m)});
}

std::string VirtualModuleManager::findModule(const std::string& name) const
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    for (const std::string& dir : moduleSearchPath) {
        std::string fullname = dir + "/mod_" + name + "/module.so";
        if (access(fullname.c_str(), R_OK)) {
            return fullname;
        }
    }
    throw std::runtime_error("Module '" + name + "' not found");
}

void VirtualModuleManager::onModuleUnload(const std::string& name)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    if (!isModuleLoaded(name)) {
        throw std::logic_error("Tried to unload not loaded virtual module: " + name);
    }
    moduleManager.unregisterModule(name);
    modules.erase(name);
}

bool VirtualModuleManager::isModuleLoaded(const std::string& name) const
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return modules.count(name) > 0;
}
