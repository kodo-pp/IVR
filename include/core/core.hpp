#ifndef CORE_CORE_HPP
#define CORE_CORE_HPP

#include <functional>
#include <string>
#include <vector>

// === Type definitions ===

using ArgsSpec = std::string;

struct FuncResult {
    std::vector<void*> data;
    uint32_t exitStatus;
};

// === FuncProvider definition ===

class FuncProvider {
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

// === Constants ===
const uint64_t RESERVED_FP_HANDLE = 0xFFFF'FFFF'4E5E'47EDull;

#endif /* end of include guard: CORE_CORE_HPP */
