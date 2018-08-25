#ifndef CORE_CORE_HPP
#define CORE_CORE_HPP

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// === Type definitions ===

using ArgsSpec = std::string;

struct FuncResult
{
    std::vector<void*> data;
};

// === FuncProvider definition ===

class FuncProvider
{
public:
    using func_type = std::function<FuncResult(const std::vector<void*>&)>;

    FuncProvider();
    FuncProvider(const std::string&, const func_type&);
    std::string getCommand() const;
    FuncResult operator()(const std::vector<void*>&);
    ~FuncProvider();

private:
    std::string command;
    func_type func;
};

// === Initialization function ===

void initilaizeCore(std::vector<std::string>&);

// === Working with FuncProviders ===

void registerFuncProvider(const FuncProvider&, ArgsSpec, ArgsSpec);

uint64_t getFuncProviderHandle(const std::string& command);

const FuncProvider& getFuncProvider(uint64_t handle);
ArgsSpec getArgsSpec(uint64_t handle);
ArgsSpec getRetSpec(uint64_t handle);

void funcProvidersCleanup();

struct ModuleClassMember
{
    char type;
};

struct ModuleClassMethod
{
    std::string return_type;
    std::string arguments_type;
};

struct ModuleClass
{
    ModuleClass(const std::unordered_map<std::string, ModuleClassMember>& _members,
                const std::unordered_map<std::string, ModuleClassMethod>& _methods,
                uint64_t parent);

    uint64_t parentId;
    std::unordered_map<std::string, uint64_t> memberHandles;
    std::unordered_map<std::string, uint64_t> methodHandles;
    std::vector<ModuleClassMember> members;
    std::vector<ModuleClassMethod> methods;
};

struct ModuleClassInstance
{
    explicit ModuleClassInstance(uint64_t handle);
    ModuleClassInstance(const ModuleClassInstance& other);
    ModuleClassInstance(ModuleClassInstance&& other) = default;
    ~ModuleClassInstance();

    uint64_t classHandle;

    mutable std::vector<void*> members;

private:
    std::shared_ptr<int> referenceCount;
};

uint64_t addModuleClass(const std::string& name, const ModuleClass& moduleClass);
void removeModuleClass(const std::string& name);
const ModuleClass& getModuleClass(uint64_t handle);
uint64_t instantiateModuleClass(uint64_t handle);
void deleteModuleClassInstance(uint64_t instanceId);

// === Constants ===
const uint64_t RESERVED_FP_HANDLE = 0xFFFF'FFFF'4E5E'47EDull;

#endif /* end of include guard: CORE_CORE_HPP */
