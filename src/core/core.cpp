#include <map>
#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <core/core.hpp>
#include <misc/die.hpp>
#include <iostream>

std::map <std::string, FuncProvider*> funcProviderMap;

// Реализация функций класса FuncProvider
FuncProvider::FuncProvider() { }
FuncProvider::FuncProvider(std::string _command, std::function <struct FuncResult * (const std::vector <void *> &) > _func) {
    command = _command;
    func = _func;
}
FuncProvider::~FuncProvider() { }

struct FuncResult * FuncProvider::operator()(const std::vector <void *> & args) {
    return func(args);
}

std::string FuncProvider::getCommand() {
    return command;
}

// Функции инициализации
static void initializeFuncProviderMap(std::vector <std::string> * args) { }

bool initilaizeCore(std::vector <std::string> * args) {
    try {
        initializeFuncProviderMap(args);
    } catch (std::runtime_error e) {
        errdie("unable to initialize function provider map", e.what());
    }
    return true;
}

bool registerFuncProvider(FuncProvider* prov) {
    if (prov == nullptr) {
        return false;
    }
    try {
        std::string command = prov->getCommand();
        if (funcProviderMap.count(command) > 0) {
            return false;
        }

        funcProviderMap.insert(make_pair(command, prov));
        return true;
    } catch (std::runtime_error e) {
        errdie("unable to register function provider", e.what());
    } catch (...) {
        errdie("unable to register function provider", "unknown error");
    }
}

FuncProvider* getFuncProvider(const std::string & command) {
    if (funcProviderMap.count(command) == 0) {
        return nullptr;
    } else {
        return funcProviderMap.at(command);
    }
}
