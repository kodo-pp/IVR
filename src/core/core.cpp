#include <util/util.hpp>
#include <core/core.hpp>
#include <log/log.hpp>
#include <misc/die.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <exception>
#include <stdexcept>
#include <tuple>
#include <mutex>

std::map < std::string, std::tuple <FuncProvider*, ArgsSpec, ArgsSpec> > funcProviderMap;
static std::recursive_mutex funcProviderMutex;

// Реализация функций класса FuncProvider
FuncProvider::FuncProvider() { }
FuncProvider::FuncProvider(std::string _command, std::function <struct FuncResult * (
                               const std::vector <void *> &) > _func) : command(_command), func(_func) { }
FuncProvider::~FuncProvider() { }

struct FuncResult * FuncProvider::operator()(const std::vector <void *> & args) {
    return func(args);
}

std::string FuncProvider::getCommand() {
    return command;
}

// Функции инициализации
// [no utf-8]
static void initializeFuncProviderMap(std::vector <std::string> * args) { }

// [no utf-8]
bool initilaizeCore(std::vector <std::string> * args) {
    try {
        initializeFuncProviderMap(args);
    } catch (std::runtime_error& e) {
        errdie("unable to initialize function provider map", e.what());
    }
    return true;
}

bool registerFuncProvider(FuncProvider* prov, ArgsSpec argsSpec, ArgsSpec retSpec) {
    std::lock_guard <std::recursive_mutex> lock(funcProviderMutex);
    if (prov == nullptr) {
        return false;
    }
    try {
        std::string command = prov->getCommand();
        if (funcProviderMap.count(command) > 0) {
            return false;
        }
        LOG(L"Registering func provider for '" << wstring_cast(command) << L"'");
        funcProviderMap.insert(std::make_pair(command, std::make_tuple(prov, argsSpec, retSpec)));
        LOG(funcProviderMap.count(command));
        return true;
    } catch (std::runtime_error& e) {
        errdie("unable to register function provider", e.what());
    } catch (...) {
        errdie("unable to register function provider", "unknown error");
    }
}

FuncProvider* getFuncProvider(const std::string & command) {
    std::lock_guard <std::recursive_mutex> lock(funcProviderMutex);
    if (funcProviderMap.count(command) == 0) {
        return nullptr;
    } else {
        return std::get<0>(funcProviderMap.at(command));
    }
}

ArgsSpec getArgsSpec(const std::string& command) {
    std::lock_guard <std::recursive_mutex> lock(funcProviderMutex);
    return std::get<1>(funcProviderMap.at(command));
}
ArgsSpec getRetSpec(const std::string& command) {
    std::lock_guard <std::recursive_mutex> lock(funcProviderMutex);
    return std::get<2>(funcProviderMap.at(command));
}

void funcProvidersCleanup() {
    std::lock_guard <std::recursive_mutex> lock(funcProviderMutex);
    for (auto& i : funcProviderMap) {
        delete std::get<0>(i.second);
    }
    funcProviderMap.clear();
}
