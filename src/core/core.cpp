#include <exception>
#include <iostream>
#include <map>
#include <mutex>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <core/core.hpp>
#include <core/dyntype.hpp>
#include <log/log.hpp>
#include <misc/die.hpp>
#include <modules/module_io.hpp>
#include <util/handle_storage.hpp>
#include <util/util.hpp>

#include <boost/algorithm/string.hpp>

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
                           std::function<struct FuncResult*(const std::vector<void*>&)> _func) :
        command(_command),
        func(_func)
{
}

// Destructor
FuncProvider::~FuncProvider()
{
}

// operator()
struct FuncResult* FuncProvider::operator()(const std::vector<void*>& args)
{
    return func(args);
}

std::string FuncProvider::getCommand()
{
    return command;
}

// === Initialization functions ===

static void initializeFuncProviderMap(UNUSED std::vector<std::string>* args)
{
    // Reserve the null handle
    auto handle = funcProviderStorage.insert({nullptr, "", ""});
    if (handle != 0ULL) {
        throw std::runtime_error("null FuncProvider handle is not zero");
    }
}

static void initializeCoreFuncProviders();

bool initilaizeCore(std::vector<std::string>* args)
{
    try {
        initializeFuncProviderMap(args);
        initializeCoreFuncProviders();
    } catch (std::exception& e) {
        errdie("unable to initialize function provider map", e.what());
    }
    return true;
}

// === Working with "FuncProvider"s ===

bool registerFuncProvider(FuncProvider* prov, ArgsSpec argsSpec, ArgsSpec retSpec)
{
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

uint64_t getFuncProviderHandle(std::string command)
{
    return funcProviderMap.at(command);
}

FuncProvider* getFuncProvider(uint64_t handle)
{
    std::lock_guard<std::recursive_mutex> lock(funcProviderMutex);
    try {
        return std::get<0>(funcProviderStorage.access(handle));
    } catch (const std::out_of_range& e) {
        return nullptr;
    }
}

ArgsSpec getArgsSpec(uint64_t handle)
{
    std::lock_guard<std::recursive_mutex> lock(funcProviderMutex);
    return std::get<1>(funcProviderStorage.access(handle));
}

ArgsSpec getRetSpec(uint64_t handle)
{
    std::lock_guard<std::recursive_mutex> lock(funcProviderMutex);
    return std::get<2>(funcProviderStorage.access(handle));
}

void funcProvidersCleanup()
{
    std::lock_guard<std::recursive_mutex> lock(funcProviderMutex);
    for (const auto& i : funcProviderMap) {
        delete std::get<0>(funcProviderStorage.access(i.second));
        funcProviderStorage.remove(i.second);
    }
    funcProviderMap.clear();
}

ModuleClass::ModuleClass(const std::unordered_map<std::string, ModuleClassMember>& _members,
                         const std::unordered_map<std::string, ModuleClassMethod>& _methods,
                         uint64_t parent = 0xFFFF'FFFF'FFFF'FFFFull) :
        parentId(parent)
{
    uint64_t counter = 0;
    for (const auto& sv : _members) {
        memberHandles.insert({sv.first, counter});
        members.emplace_back(sv.second);
        ++counter;
    }
    if (parent != 0xFFFF'FFFF'FFFF'FFFFull) {
        for (const auto& sh : getModuleClass(parent).memberHandles) {
            memberHandles.insert({sh.first, counter});
            members.emplace_back(getModuleClass(parent).members.at(sh.second));
            ++counter;
        }
    }
    counter = 0;
    for (const auto& kv : _methods) {
        methodHandles.insert({kv.first, counter});
        methods.emplace_back(kv.second);
        ++counter;
    }
    if (parent != 0xFFFF'FFFF'FFFF'FFFFull) {
        for (const auto& sh : getModuleClass(parent).methodHandles) {
            methodHandles.insert({sh.first, counter});
            methods.emplace_back(getModuleClass(parent).methods.at(sh.second));
            ++counter;
        }
    }
}

ModuleClassInstance::ModuleClassInstance(uint64_t handle)
{
    const auto& memberTypes = getModuleClass(handle).members;
    members.resize(memberTypes.size(), nullptr);
    for (uint64_t i = 0; i < members.size(); ++i) {
        members.at(i) = dyntypeNew(memberTypes.at(i).type);
    }
    classHandle = handle;
}

ModuleClassInstance::~ModuleClassInstance()
{
    const auto& memberTypes = getModuleClass(classHandle).members;
    members.resize(memberTypes.size(), nullptr);
    for (uint64_t i = 0; i < members.size(); ++i) {
        dyntypeDelete(members.at(i), memberTypes.at(i).type);
    }
}

std::recursive_mutex moduleClassMutex;

static std::unordered_map<std::string, uint64_t> moduleClassHandles;
static HandleStorage<uint64_t, ModuleClass> moduleClasses;

static HandleStorage<uint64_t, ModuleClassInstance> moduleClassInstances;

uint64_t addModuleClass(const std::string& name, const ModuleClass& moduleClass)
{
    std::lock_guard<std::recursive_mutex> lock(moduleClassMutex);
    auto handle = moduleClasses.insert(moduleClass);
    moduleClassHandles.insert({name, handle});
    return handle;
}

void removeModuleClass(const std::string& name)
{
    std::lock_guard<std::recursive_mutex> lock(moduleClassMutex);
    auto handle = moduleClassHandles.at(name);
    moduleClassHandles.erase(name);
    moduleClasses.remove(handle);
}

const ModuleClass& getModuleClass(uint64_t handle)
{
    std::lock_guard<std::recursive_mutex> lock(moduleClassMutex);
    return moduleClasses.access(handle);
}

uint64_t instantiateModuleClass(uint64_t handle)
{
    std::lock_guard<std::recursive_mutex> lock(moduleClassMutex);
    return moduleClassInstances.insert(ModuleClassInstance(handle));
}

void deleteModuleClassInstance(uint64_t instanceId)
{
    std::lock_guard<std::recursive_mutex> lock(moduleClassMutex);
    moduleClassInstances.remove(instanceId);
}

FuncResult* handlerAddModuleClass(const std::vector<void*>& args)
{
    if (args.size() != 4) {
        throw std::logic_error("Wrong number of arguments for handlerAddModuleClass()");
    }
    auto ret = new struct FuncResult;
    ret->data.resize(1);
    try {
        uint64_t parent = getArgument<uint64_t>(args, 0);
        std::string name = getArgument<std::string>(args, 1);
        std::string members = getArgument<std::string>(args, 2);
        std::string methods = getArgument<std::string>(args, 3);

        std::vector<std::string> memberNames;
        std::unordered_map<std::string, ModuleClassMember> memberMap;
        boost::algorithm::split(memberNames, members, [](char c) { return c == ':'; });
        for (auto& i : memberNames) {
            char type = i.back();
            i.pop_back();
            memberMap.insert({i, {type}});
        }

        std::vector<std::string> methodDefs;
        std::unordered_map<std::string, ModuleClassMethod> methodMap;
        boost::algorithm::split(methodDefs, methods, [](char c) { return c == ':'; });
        for (const auto& i : methodDefs) {
            std::vector<std::string> tmp;
            boost::algorithm::split(tmp, i, [](char c) { return c == ','; });
            if (tmp.size() != 3) {
                throw std::runtime_error("invalid number of comma-separated arguments");
            }
            methodMap.insert({tmp[0], {tmp[1], tmp[2]}});
        }

        auto handle = addModuleClass(name, ModuleClass(memberMap, methodMap, parent));
        setReturn<uint64_t>(ret, 0, handle);

    } catch (const std::exception& e) {
        LOG("Exception caught: " << e.what());
        ret->exitStatus = 1;
        return ret;
    }
    ret->exitStatus = 0;
    return ret;
}

// void handlerRemoveModuleClass(const std::string& name);
FuncResult* handlerInstantiateModuleClass(const std::vector<void*>& args)
{
    if (args.size() != 1) {
        throw std::logic_error("Wrong number of arguments for handlerInstantiateModuleClass()");
    }
    auto ret = new struct FuncResult;
    ret->data.resize(1);
    try {
        uint64_t classHandle = getArgument<uint64_t>(args, 0);
        auto handle = instantiateModuleClass(classHandle);
        setReturn<uint64_t>(ret, 0, handle);

    } catch (const std::exception& e) {
        LOG("Exception caught: " << e.what());
        ret->exitStatus = 1;
        return ret;
    }
    ret->exitStatus = 0;
    return ret;
}

FuncResult* handlerModuleClassSetString(const std::vector<void*>& args)
{
    LOG("SETSTRING");
    if (args.size() != 3) {
        throw std::logic_error("Wrong number of arguments for handlerModuleClassSetString()");
    }
    auto ret = new struct FuncResult;
    try {
        auto instanceHandle = getArgument<uint64_t>(args, 0);
        auto memberHandle = getArgument<uint64_t>(args, 1);
        auto value = getArgument<std::string>(args, 2);

        if (moduleClasses.access(moduleClassInstances.access(instanceHandle).classHandle)
                    .members.at(memberHandle)
                    .type
            != 's') {
            LOG("Type mismatch: expected s, got "
                << moduleClasses.access(moduleClassInstances.access(instanceHandle).classHandle)
                           .members.at(memberHandle)
                           .type);
            throw std::runtime_error("type mismatch: expected string");
        }
        *static_cast<std::string*>(
                moduleClassInstances.mutableAccess(instanceHandle).members.at(memberHandle))
                = value;
    } catch (const std::exception& e) {
        LOG("Exception caught: " << e.what());
        ret->exitStatus = 1;
        return ret;
    }
    ret->exitStatus = 0;
    return ret;
}

FuncResult* handlerModuleClassGetString(const std::vector<void*>& args)
{
    if (args.size() != 2) {
        throw std::logic_error("Wrong number of arguments for handlerModuleClassGetString()");
    }
    auto ret = new struct FuncResult;
    ret->data.resize(1);
    try {
        auto instanceHandle = getArgument<uint64_t>(args, 0);
        auto memberHandle = getArgument<uint64_t>(args, 1);

        if (moduleClasses.access(moduleClassInstances.access(instanceHandle).classHandle)
                    .members.at(memberHandle)
                    .type
            != 's') {
            LOG("Type mismatch: expected s, got "
                << moduleClasses.access(moduleClassInstances.access(instanceHandle).classHandle)
                           .members.at(memberHandle)
                           .type);
            throw std::runtime_error("type mismatch: expected string");
        }
        std::string value = *static_cast<std::string*>(
                moduleClassInstances.access(instanceHandle).members.at(memberHandle));

        setReturn<std::string>(ret, 0, value);
    } catch (const std::exception& e) {
        LOG("Exception caught: " << e.what());
        ret->exitStatus = 1;
        return ret;
    }
    ret->exitStatus = 0;
    return ret;
}
// void handlerDeleteModuleClassInstance(uint64_t handle);

static void initializeCoreFuncProviders()
{
    registerFuncProvider(new FuncProvider("core.class.add", handlerAddModuleClass), "Lsss", "L");
    registerFuncProvider(
            new FuncProvider("core.class.instantiate", handlerInstantiateModuleClass), "L", "L");
    registerFuncProvider(
            new FuncProvider("core.class.setString", handlerModuleClassSetString), "LLs", "");
    registerFuncProvider(
            new FuncProvider("core.class.getString", handlerModuleClassGetString), "LL", "s");
}
