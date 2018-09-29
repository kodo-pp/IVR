#include <thread>

#include <modbox/core/virtual_module.hpp>

VirtualModule::VirtualModule(const std::string& path) : dso(path)
{
}

void VirtualModule::spawnModuleThread(const std::vector<std::string>& arguments)
{
    try {
        LOG("VirtualModule::spawnModuleThread");
        std::thread thr([this, arguments]() {
            dso.resolve<void, const std::vector<std::string>&>("module_main")(arguments);
        });
        thr.detach();
        LOG("VirtualModule::spawnModuleThread finished");
    } catch (const std::exception& e) {
        LOG("VirtualModule::spawnModuleThread: std::exception: " << wstring_cast(e.what()));
    }
}
