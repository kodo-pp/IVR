#include <map>
#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <core/core.hpp>
#include <iostream>
#include <log/log.hpp>
#include <misc/die.hpp>
#include <tuple>
#include <util/util.hpp>

std::map < std::string, std::tuple <FuncProvider*, ArgsSpec, ArgsSpec> > funcProviderMap;

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
        // STOPPED HERE
        return true;
    } catch (std::runtime_error& e) {
        errdie("unable to register function provider", e.what());
    } catch (...) {
        errdie("unable to register function provider", "unknown error");
    }
}

FuncProvider* getFuncProvider(const std::string & command) {
    if (funcProviderMap.count(command) == 0) {
        return nullptr;
    } else {
        return std::get<0>(funcProviderMap.at(command));
    }
}

ArgsSpec getArgsSpec(const std::string& command) {
    return std::get<1>(funcProviderMap.at(command));
}
ArgsSpec getRetSpec(const std::string& command) {
    return std::get<2>(funcProviderMap.at(command));
}

void funcProvidersCleanup() {
    for (auto& i : funcProviderMap) {
        delete std::get<0>(i.second);
    }
    funcProviderMap.clear();
}
