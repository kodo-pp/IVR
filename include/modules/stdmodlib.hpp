#ifndef MODULES_STDMODLIB_HPP
#define MODULES_STDMODLIB_HPP

#include <map>
#include <modules/module.hpp>
#include <vector>

// TODO: make this a separate header file
using Version = std::vector<int>;

/**
 * Represents the standard module library
 */
class StdModLib {
public:
    StdModLib(Version _version, std::map<ModuleId, Module> _modules);

    // TODO: some operations like addModule, removeModule instead of returning by reference
    std::map<ModuleId, Module>& getModules();

    Version getVersion();
    void setVersion(Version newVersion);

protected:
    Version version;
    std::map<ModuleId, Module> modules;
};

#endif /* end of include guard: MODULES_STDMODLIB_HPP */
