#include <core/core.hpp>
#include <exception>
#include <iostream>
#include <log/log.hpp>
#include <map>
#include <misc/die.hpp>
#include <mutex>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <util/handle_storage.hpp>
#include <util/util.hpp>
#include <vector>

// === Static variables ===

// Map: command -> handle
std::unordered_map<std::string, uint64_t> funcProviderMap;

// Storage of FuncProvider handles
HandleStorage<uint64_t, std::tuple<FuncProvider*, ArgsSpec, ArgsSpec>> funcProviderStorage;

// Mutex protecting the access to FuncProvider functions
static std::recursive_mutex funcProviderMutex;

// === Implementation of FuncProvider methods ===

// Default constructor
FuncProvider::FuncProvider() = default;

// AWW, this codestyle is awful. TODO: make it look beautiful
// Command-and-function constructor
FuncProvider::FuncProvider(std::string _command,
                           std::function<struct FuncResult*(const std::vector<void*>&)> _func)
        : command(_command), func(_func) {}

// Destructor
FuncProvider::~FuncProvider() {}

// operator()
struct FuncResult* FuncProvider::operator()(const std::vector<void*>& args) {
    return func(args);
}

std::string FuncProvider::getCommand() { return command; }

// === Initialization functions ===

static void initializeFuncProviderMap(std::vector<std::string>* args) {
    // Reserve the null handle
    auto handle = funcProviderStorage.insert({nullptr, "", ""});
    if (handle != 0ULL) {
        throw std::runtime_error("null FuncProvider handle is not zero");
    }
}

bool initilaizeCore(std::vector<std::string>* args) {
    try {
        initializeFuncProviderMap(args);
    } catch (std::exception& e) {
        errdie("unable to initialize function provider map", e.what());
    }
    return true;
}

// === Working with "FuncProvider"s ===

bool registerFuncProvider(FuncProvider* prov, ArgsSpec argsSpec, ArgsSpec retSpec) {
    std::lock_guard<std::recursive_mutex> lock(funcProviderMutex);
    if (prov == nullptr) {
        return false;
    }

    try {
        std::string command = prov->getCommand();
        if (funcProviderMap.count(command) > 0) {
            // If the command is already in use
            return false;
        }

        LOG(L"Registering func provider for '" << wstring_cast(command) << L"'");
        auto handle = funcProviderStorage.insert(std::make_tuple(prov, argsSpec, retSpec));
        funcProviderMap.insert({command, handle});
        LOG(L"Successfully registered FuncProvider for " << wstring_cast(command));

        return true;
    } catch (std::runtime_error& e) {
        errdie("unable to register function provider", e.what());
    } catch (...) {
        errdie("unable to register function provider", "unknown error");
    }
}

uint64_t getFuncProviderHandle(std::string command) { return funcProviderMap.at(command); }

FuncProvider* getFuncProvider(uint64_t handle) {
    std::lock_guard<std::recursive_mutex> lock(funcProviderMutex);
    try {
        return std::get<0>(funcProviderStorage.access(handle));
    } catch (const std::out_of_range& e) {
        return nullptr;
    }
}

ArgsSpec getArgsSpec(uint64_t handle) {
    std::lock_guard<std::recursive_mutex> lock(funcProviderMutex);
    return std::get<1>(funcProviderStorage.access(handle));
}

ArgsSpec getRetSpec(uint64_t handle) {
    std::lock_guard<std::recursive_mutex> lock(funcProviderMutex);
    return std::get<2>(funcProviderStorage.access(handle));
}

void funcProvidersCleanup() {
    std::lock_guard<std::recursive_mutex> lock(funcProviderMutex);
    for (const auto& i : funcProviderMap) {
        delete std::get<0>(funcProviderStorage.access(i.second));
        funcProviderStorage.remove(i.second);
    }
    funcProviderMap.clear();
}
