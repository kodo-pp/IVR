#ifndef CORE_VIRTUAL_MODULE_HPP
#define CORE_VIRTUAL_MODULE_HPP

#include <string>

#include <modbox/util/dso.hpp>

class VirtualModule
{
public:
    VirtualModule(const std::string& path);
    VirtualModule(const VirtualModule& other) = delete;
    VirtualModule(VirtualModule&& other) = default;
    virtual ~VirtualModule() = default;

    VirtualModule& operator=(const VirtualModule& other) = delete;
    VirtualModule& operator=(VirtualModule&& other) = default;

    void spawnModuleThread(const std::vector<std::string>& arguments = {});

private:
    Dso dso;
};

#endif /* end of include guard: CORE_VIRTUAL_MODULE_HPP */
