#ifndef CORE_CORE_HPP
#define CORE_CORE_HPP

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

// === Type definitions ===

using ArgsSpec = std::string;

struct FuncResult {
    std::vector<void*> data;
    uint32_t exitStatus;
};

// === FuncProvider definition ===

class FuncProvider
{
public:
    FuncProvider();
    FuncProvider(std::string, std::function<struct FuncResult*(const std::vector<void*>&)>);
    std::string getCommand();
    struct FuncResult* operator()(const std::vector<void*>&);
    ~FuncProvider();

private:
    std::string command;
    std::function<struct FuncResult*(const std::vector<void*>&)> func;
};

// === Initialization function ===

bool initilaizeCore(std::vector<std::string>*);

// === Working with FuncProviders ===

bool registerFuncProvider(FuncProvider*, ArgsSpec, ArgsSpec);

uint64_t getFuncProviderHandle(std::string command);

FuncProvider* getFuncProvider(uint64_t handle);
ArgsSpec getArgsSpec(uint64_t handle);
ArgsSpec getRetSpec(uint64_t handle);

void funcProvidersCleanup();

struct ModuleClassMember {
    char type;
};

struct ModuleClassMethod {
    std::string return_type;
    std::string arguments_type;
};

struct ModuleClass {
    ModuleClass(const std::unordered_map<std::string, ModuleClassMember>& _members,
                const std::unordered_map<std::string, ModuleClassMethod>& _methods,
                uint64_t parent);
    uint64_t parentId;
    std::unordered_map<std::string, uint64_t> memberHandles;
    std::unordered_map<std::string, uint64_t> methodHandles;
    std::vector<ModuleClassMember> members;
    std::vector<ModuleClassMethod> methods;
};

struct ModuleClassInstance {
    explicit ModuleClassInstance(uint64_t handle);
    ~ModuleClassInstance();
    uint64_t classHandle;
    std::vector<void*> members;
};

uint64_t addModuleClass(const std::string& name, const ModuleClass& moduleClass);
void removeModuleClass(const std::string& name);
const ModuleClass& getModuleClass(uint64_t handle);
uint64_t instantiateModuleClass(uint64_t handle);
void deleteModuleClassInstance(uint64_t handle);

// === Constants ===
const uint64_t RESERVED_FP_HANDLE = 0xFFFF'FFFF'4E5E'47EDull;

#endif /* end of include guard: CORE_CORE_HPP */
