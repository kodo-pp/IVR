#ifndef CORE_VIRTUAL_MODULE_HPP
#define CORE_VIRTUAL_MODULE_HPP

#include <string>

#include <modbox/modules/module.hpp>
#include <modbox/util/dso.hpp>

class VirtualModule
{
public:
    explicit VirtualModule(const std::string& path);
    VirtualModule(const VirtualModule& other) = delete;
    VirtualModule(VirtualModule&& other) = default;
    virtual ~VirtualModule() = default;

    VirtualModule& operator=(const VirtualModule& other) = delete;
    VirtualModule& operator=(VirtualModule&& other) = default;

    const Module& getModule() const;
    void spawnModuleThread(const std::vector<std::string>& arguments = {});

private:
    Dso dso;
    Module module;
};

class VirtualModuleManager
{
public:
    void load(const std::string& vmodName);
    VirtualModule& get(const std::string& vmodName);
    const VirtualModule& get(const std::string& vmodName) const;

private:
    std::unordered_map<std::string, VirtualModule> modules;
};

class VirtualModuleManager
{
public:
    VirtualModuleManager();
    VirtualModuleManager(const VirtualModuleManager& other) = delete;
    VirtualModuleManager(VirtualModuleManager&& other) = default;
    virtual ~VirtualModuleManager() = default;

    VirtualModuleManager& operator=(const VirtualModuleManager& other) = delete;
    VirtualModuleManager& operator=(VirtualModuleManager&& other) = default;

    void loadModule(const std::string& name, std::vector<std::string> args = {});
    void onModuleUnload(const std::string& name);
    bool isModuleLoaded(const std::string& name) const;
    std::string findModule(const std::string& name) const;

private:
    const std::vector<std::string> moduleSearchPath = {"modules", "/usr/share/modbox/modules"};
    std::unordered_map<std::string, VirtualModule> modules;
    mutable std::recursive_mutex mutex;
};

extern VirtualModuleManager vmodManager;

#endif /* end of include guard: CORE_VIRTUAL_MODULE_HPP */
