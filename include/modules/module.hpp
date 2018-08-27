#ifndef MODULES_MODULE_HPP
#define MODULES_MODULE_HPP

#include <memory>
#include <mutex>
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
    Module(int _mainSocket,
           int _reverseSocket,
           const std::wstring& _name,
           ModuleId _id,
           const ModuleLanguage& _language);
    Module(const Module& other);
    Module(Module&& other) = default;

    Module& operator=(const Module& other); //
    Module& operator=(Module&& other) = default;

    // Закрыть все сокеты
    void cleanup() noexcept;

    // Эту работу выполняет moduleListenerThread (и ModuleWorker)
    // void load();
    // void unload();

    ModuleId getModuleId() const;
    // Not sure if id should be a constant, so no setModuleId() yet

    std::wstring getName() const;
    void setName(const std::wstring& newName);

    std::wstring getExecutableFileName() const;
    void setExecutableFileName(const std::wstring& newExecFileName);

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
    mutable std::recursive_mutex mtx;
};

#endif /* end of include guard: MODULES_MODULE_HPP */
