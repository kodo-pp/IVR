#include <thread>

#include <modbox/core/virtual_module.hpp>
#include <modbox/modules/module_manager.hpp>

VirtualModuleManager vmodManager;

VirtualModule::VirtualModule(const std::string& path, const std::string& name)
        : dso(path), module(-1, -1, name, {})
{
    LOG("Loading virtual module (dso.filename = '" << dso.getFilename() << "')");
}

VirtualModule::~VirtualModule()
{
    LOG("Deleting virtual module (dso.filename = '" << dso.getFilename() << "')");
    vmodManager.onModuleUnload(module.getName());
}

void VirtualModule::spawnModuleThread(const std::vector<std::string>& arguments)
{
    try {
        LOG("VirtualModule::spawnModuleThread");
        std::thread thr([this, arguments]() {
            dso.resolve<void, const std::vector<std::string>&>("module_main")(arguments);
            LOG("module exited");
            vmodManager.triggerModuleUnloadThreadUnsafe(arguments.at(0));
            LOG("module unload triggered");

            // vmodManager.onModuleUnload(arguments.at(0));
            // LOG("onModuleUnload() exited");
        });
        // thr.detach();
        thr.join();
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
    VirtualModule m(findModule(name), name);
    moduleManager.registerModule(m.getModule());
    m.spawnModuleThread(args);
    modules.insert({name, std::move(m)});
}

std::string VirtualModuleManager::findModule(const std::string& name) const
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    std::string fullname = "modules/virtual/" + name + "/module.so";
    LOG("access(" << fullname << ")");
    if (access(fullname.c_str(), R_OK) == 0) {
        return fullname;
    } else {
        throw std::runtime_error("Module '" + name + "' not found");
    }
}

void VirtualModuleManager::onModuleUnload(const std::string& name)
{
    LOG("Acquiring lock... (onModuleUnload)");
    std::lock_guard<std::recursive_mutex> lock(mutex);
    LOG("Acquired lock (onModuleUnload)");
    if (!isModuleLoaded(name)) {
        throw std::logic_error("Tried to unload not loaded virtual module: " + name);
    }
    moduleManager.unregisterModule(name);
}

bool VirtualModuleManager::isModuleLoaded(const std::string& name) const
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return modules.count(name) > 0;
}

void VirtualModuleManager::triggerModuleUnloadThreadUnsafe(const std::string& moduleName)
{
    modules.erase(moduleName);
}
