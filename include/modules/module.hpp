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

class Module
{
public:
    Module(int _mainSocket, int _reverseSocket);
    Module(const Module& other) = default; // COMBAK: может быть, убрать copy ctor
    Module(Module&& other) = default;

    Module& operator=(const Module& other) = default;
    Module& operator=(Module&& other) = default;

    // Закрыть все сокеты
    void cleanup();

    // Эту работу выполняет moduleListenerThread (и ModuleWorker)
    // void load();
    // void unload();

    ModuleId getModuleId() const;
    // Not sure if id should be a constant, so no setModuleId() yet

    std::wstring getName() const;
    void setName(std::wstring newName);

    std::wstring getExecutableFileName() const;
    void setExecutableFileName(std::wstring newExecFileName);

    int getMainSocket() const;
    int getReverseSocket() const;

protected:
    int mainSocket;
    int reverseSocket;
    ModuleId id;
    std::wstring name;
    std::wstring executableFileName;
    ModuleLanguage language; // Programming language, not sure how it'll be used,
                             // maybe in automatic setup of necessary tools
};

#endif /* end of include guard: MODULES_MODULE_HPP */
