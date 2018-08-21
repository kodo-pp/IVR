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

bool initilaizeCore(std::vector<std::string>* args)
{
    try {
        initializeFuncProviderMap(args);
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
                         uint64_t parent) :
        parentId(parent)
{
    uint64_t counter = 0;
    for (const auto& sv : _members) {
        memberHandles.insert({sv.first, counter});
        members.emplace_back(sv.second);
        ++counter;
    }
    for (const auto& sh : getModuleClass(parent).memberHandles) {
        memberHandles.insert({sh.first, counter});
        members.emplace_back(getModuleClass(parent).members.at(sh.second));
        ++counter;
    }
    counter = 0;
    for (const auto& kv : _methods) {
        methodHandles.insert({kv.first, counter});
        methods.emplace_back(kv.second);
        ++counter;
    }
    for (const auto& sh : getModuleClass(parent).methodHandles) {
        methodHandles.insert({sh.first, counter});
        methods.emplace_back(getModuleClass(parent).methods.at(sh.second));
        ++counter;
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
        throw std::logic_error("Wrong number of arguments for handlerGraphicsMoveObject()");
    }
    auto ret = new struct FuncResult;

    uint64_t parent = getArgument<uint64_t>(args, 0);
    std::string members = getArgument<std::string>(args, 1);
    std::string methods = getArgument<std::string>(args, 2);

    std::vector<std::string> memberNames;
    std::vector<char> memberTypes;
    boost::algorithm::split(memberNames, members, ':');
    memberTypes.reserve(memberNames.size());
    for (auto& i : memberNames) {
        memberTypes.emplace_back(i.back());
        i.pop_back();
    }
    std::vector<std::string> methodDefs;
    std::vector<std::string> methodNames;
    std::vector<std::string> methodRetTypes;
    std::vector<std::string> methodArgTypes;
    boost::algorithm::split(methodDefs, methods, ':');
    methodNames.reserve(methodDefs.size());
    methodRetTypes.reserve(methodDefs.size());
    methodArgTypes.reserve(methodDefs.size());

    for (const auto& i : methodDefs) {
        std::array<std::string, 3> tmp;
        boost::algorithm::split(tmp, i, ',');
        methodNames.emplace_back(tmp[0]);
        methodRetTypes.emplace_back(tmp[1]);
        methodArgTypes.emplace_back(tmp[2]);
    }

    ret->exitStatus = 0;
    return ret;
}
void handlerRemoveModuleClass(const std::string& name);
const ModuleClass& handlerGetModuleClass(uint64_t handle);
uint64_t handlerInstantiateModuleClass(uint64_t handle);
void handlerDeleteModuleClassInstance(uint64_t handle);
