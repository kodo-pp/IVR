#ifndef MODULES_MODULE_HPP
#define MODULES_MODULE_HPP

#include <string>

// enum is useless in this case as we should support any programming language,
// which may be not listed in this enum
using ModuleLanguage = std::wstring;
using ModuleId = uint64_t;

/**
 * Represents a module
 */

class Module {
public:
    void load();
    void unload();

    ModuleId getModuleId();
    // Not sure if id should be a constant, so no setModuleId() yet

    std::wstring getName();
    void setName(std::wstring newName);

    std::wstring getExecutableFileName();
    void setExecutableFileName(std::wstring newExecFileName);
protected:
    ModuleId id;
    std::wstring name;
    std::wstring executableFileName;
    ModuleLanguage language; // Programming language, not sure how it'll be used,
                             // maybe in automatic setup of necessary tools
};


#endif /* end of include guard: MODULES_MODULE_HPP */
