#include <algorithm>
#include <exception>
#include <iostream>
#include <map>
#include <mutex>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <core/core.hpp>
#include <core/dyntype.hpp>
#include <log/log.hpp>
#include <misc/die.hpp>
#include <modules/module_io.hpp>
#include <modules/module_manager.hpp>
#include <util/handle_storage.hpp>
#include <util/util.hpp>

#include <boost/algorithm/string.hpp>

// === Static variables ===

// Map: command -> handle
std::unordered_map<std::string, uint64_t> funcProviderMap;

// Storage of FuncProvider handles
HandleStorage<uint64_t, std::tuple<FuncProvider, ArgsSpec, ArgsSpec>> funcProviderStorage;

// Mutex protecting the access to FuncProvider functions
static std::recursive_mutex funcProviderMutex;

// === Implementation of FuncProvider methods ===

// Default constructor
FuncProvider::FuncProvider() = default;

// AWW, this codestyle is awful. TODO: make it look beautiful
// Command-and-function constructor
FuncProvider::FuncProvider(const std::string& _command,
                           const std::function<struct FuncResult(const std::vector<void*>&)>& _func)
        : command(_command), func(_func)
{
}

// Destructor
FuncProvider::~FuncProvider()
{
}

// operator()
FuncResult FuncProvider::operator()(const std::vector<void*>& args)
{
    return func(args);
}

std::string FuncProvider::getCommand() const
{
    return command;
}

// === Initialization functions ===

static void initializeFuncProviderMap(UNUSED std::vector<std::string>& args)
{
    // Reserve the null handle
    auto handle = funcProviderStorage.insert(
            {FuncProvider("RESERVED",
                          FuncProvider::func_type([](const std::vector<void*>&) -> FuncResult {
                              throw std::logic_error("zero FuncProvider called");
                          })),
             "",
             ""});
    if (handle != 0ULL) {
        throw std::logic_error("zero FuncProvider handle is not zero");
    }
}

static void initializeCoreFuncProviders();

void initilaizeCore(std::vector<std::string>& args)
{
    initializeFuncProviderMap(args);
    initializeCoreFuncProviders();
}

// === Working with "FuncProvider"s ===

void registerFuncProvider(const FuncProvider& prov, ArgsSpec argsSpec, ArgsSpec retSpec)
{
    std::lock_guard<std::recursive_mutex> lock(funcProviderMutex);

    std::string command = prov.getCommand();
    if (funcProviderMap.count(command) > 0) {
        throw std::runtime_error("The command for FuncProvider is already in use");
    }

    LOG(L"Registering func provider for '" << wstring_cast(command) << L"'");
    auto handle = funcProviderStorage.insert({prov, argsSpec, retSpec});
    funcProviderMap.insert({command, handle});
    LOG(L"Successfully registered FuncProvider for " << wstring_cast(command));
}

uint64_t getFuncProviderHandle(const std::string& command)
{
    return funcProviderMap.at(command);
}
const FuncProvider& getFuncProvider(uint64_t handle)
{
    std::lock_guard<std::recursive_mutex> lock(funcProviderMutex);
    try {
        return std::get<0>(funcProviderStorage.access(handle));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error(std::string("No such FuncProvider handle: ")
                                 + std::to_string(handle));
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
        funcProviderStorage.remove(i.second);
    }
    funcProviderMap.clear();
}

ModuleClass::ModuleClass(const std::unordered_map<std::string, ModuleClassMember>& _members,
                         const std::unordered_map<std::string, ModuleClassMethod>& _methods,
                         uint64_t parent = 0xFFFF'FFFF'FFFF'FFFFull)
        : parentId(parent)
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
        : classHandle(handle), referenceCount(std::make_shared<int>(1))
{
    const auto& memberTypes = getModuleClass(handle).members;
    members.resize(memberTypes.size(), nullptr);
    for (uint64_t i = 0; i < members.size(); ++i) {
        members.at(i) = dyntypeNew(memberTypes.at(i).type);
    }
    classHandle = handle;
}

// TODO: make copy-ctor not a move one

ModuleClassInstance::ModuleClassInstance(const ModuleClassInstance& other)
        : classHandle(other.classHandle)
        , members(other.members)
        , referenceCount(other.referenceCount)
{
    ++(*referenceCount);
    // TODO: make move constructor from it
    //
    // const auto& memberTypes = getModuleClass(other.classHandle).members;
    // members.resize(memberTypes.size(), nullptr);
    // for (uint64_t i = 0; i < members.size(); ++i) {
    //     members.at(i) = dyntypeNew(memberTypes.at(i).type);
    //     std::swap(members.at(i), other.members.at(i));
    // }
    // classHandle = other.classHandle;
}

ModuleClassInstance::~ModuleClassInstance()
{
    if (referenceCount == nullptr) {
        return;
    }
    --(*referenceCount);
    if (*referenceCount == 0) {
        const auto& memberTypes = getModuleClass(classHandle).members;
        members.resize(memberTypes.size(), nullptr);
        for (uint64_t i = 0; i < members.size(); ++i) {
            dyntypeDelete(members.at(i), memberTypes.at(i).type);
        }
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

FuncResult handlerAddModuleClass(const std::vector<void*>& args)
{
    if (args.size() != 4) {
        throw std::logic_error("Wrong number of arguments for handlerAddModuleClass()");
    }
    FuncResult ret;
    ret.data.resize(1);
    uint64_t parent = getArgument<uint64_t>(args, 0);
    std::string name = getArgument<std::string>(args, 1);
    std::string members = getArgument<std::string>(args, 2);
    std::string methods = getArgument<std::string>(args, 3);

    std::vector<std::string> memberNames;
    std::unordered_map<std::string, ModuleClassMember> memberMap;
    boost::algorithm::split(memberNames, members, [](char c) { return c == ':'; });
    for (auto& i : memberNames) {
        if (i.empty()) {
            continue;
        }
        char type = i.back();
        i.pop_back();
        memberMap.insert({i, {type}});
    }

    std::vector<std::string> methodDefs;
    std::unordered_map<std::string, ModuleClassMethod> methodMap;
    boost::algorithm::split(methodDefs, methods, [](char c) { return c == ':'; });
    for (const auto& i : methodDefs) {
        if (i.empty()) {
            continue;
        }
        std::vector<std::string> tmp;
        boost::algorithm::split(tmp, i, [](char c) { return c == ','; });
        if (tmp.size() != 4) {
            throw std::runtime_error("invalid number of comma-separated arguments");
        }
        methodMap.insert({tmp[0], {tmp[1], tmp[2], tmp[3]}});
    }

    auto handle = addModuleClass(name, ModuleClass(memberMap, methodMap, parent));
    setReturn<uint64_t>(ret, 0, handle);

    return ret;
}

// void handlerRemoveModuleClass(const std::string& name);
FuncResult handlerInstantiateModuleClass(const std::vector<void*>& args)
{
    if (args.size() != 1) {
        throw std::logic_error("Wrong number of arguments for handlerInstantiateModuleClass()");
    }
    FuncResult ret;
    ret.data.resize(1);
    uint64_t classHandle = getArgument<uint64_t>(args, 0);
    auto handle = instantiateModuleClass(classHandle);
    setReturn<uint64_t>(ret, 0, handle);

    return ret;
}

#define HANDLER_MODCLASS_ACCESSOR(typeWord, typeNameCxx, typeChar)                                 \
    FuncResult handlerModuleClassSet##typeWord(const std::vector<void*>& args)                     \
    {                                                                                              \
        if (args.size() != 3) {                                                                    \
            throw std::logic_error("Wrong number of arguments for handlerModuleClassSet" #typeWord \
                                   "()");                                                          \
        }                                                                                          \
        FuncResult ret;                                                                            \
        auto instanceHandle = getArgument<uint64_t>(args, 0);                                      \
        auto memberHandle = getArgument<uint64_t>(args, 1);                                        \
        auto value = getArgument<typeNameCxx>(args, 2);                                            \
                                                                                                   \
        if (moduleClasses.access(moduleClassInstances.access(instanceHandle).classHandle)          \
                    .members.at(memberHandle)                                                      \
                    .type                                                                          \
            != typeChar) {                                                                         \
            throw std::runtime_error("type mismatch: expected: " #typeWord);                       \
        }                                                                                          \
        *static_cast<typeNameCxx*>(                                                                \
                moduleClassInstances.mutableAccess(instanceHandle).members.at(memberHandle))       \
                = value;                                                                           \
        return ret;                                                                                \
    }                                                                                              \
                                                                                                   \
    FuncResult handlerModuleClassGet##typeWord(const std::vector<void*>& args)                     \
    {                                                                                              \
        if (args.size() != 2) {                                                                    \
            throw std::logic_error("Wrong number of arguments for handlerModuleClassGet" #typeWord \
                                   "()");                                                          \
        }                                                                                          \
        FuncResult ret;                                                                            \
        ret.data.resize(1);                                                                        \
        auto instanceHandle = getArgument<uint64_t>(args, 0);                                      \
        auto memberHandle = getArgument<uint64_t>(args, 1);                                        \
                                                                                                   \
        if (moduleClasses.access(moduleClassInstances.access(instanceHandle).classHandle)          \
                    .members.at(memberHandle)                                                      \
                    .type                                                                          \
            != typeChar) {                                                                         \
            throw std::runtime_error("type mismatch: expected: " #typeWord);                       \
        }                                                                                          \
        typeNameCxx value = *static_cast<typeNameCxx*>(                                            \
                moduleClassInstances.access(instanceHandle).members.at(memberHandle));             \
                                                                                                   \
        setReturn<typeNameCxx>(ret, 0, value);                                                     \
        return ret;                                                                                \
    }

// Здесь можно было бы написать шаблон...
// Но я уже написал макрос

HANDLER_MODCLASS_ACCESSOR(String, std::string, 's')
HANDLER_MODCLASS_ACCESSOR(Int8, int8_t, 'b')
HANDLER_MODCLASS_ACCESSOR(Int16, int16_t, 'h')
HANDLER_MODCLASS_ACCESSOR(Int32, int32_t, 'i')
HANDLER_MODCLASS_ACCESSOR(Int64, int64_t, 'l')
HANDLER_MODCLASS_ACCESSOR(UInt8, uint8_t, 'B')
HANDLER_MODCLASS_ACCESSOR(UInt16, uint16_t, 'H')
HANDLER_MODCLASS_ACCESSOR(UInt32, uint32_t, 'I')
HANDLER_MODCLASS_ACCESSOR(UInt64, uint64_t, 'L')
HANDLER_MODCLASS_ACCESSOR(Float32, float, 'f')
HANDLER_MODCLASS_ACCESSOR(Float64, double, 'F')
HANDLER_MODCLASS_ACCESSOR(WideString, std::wstring, 'w')
HANDLER_MODCLASS_ACCESSOR(Blob, std::string, 'o')
#undef HANDLER_MODCLASS_ACCESSOR

#define HANDLER_MODCLASS_ACCESSOR_REGISTER(typeWord, typeChar)                                     \
    registerFuncProvider(                                                                          \
            FuncProvider("core.class.instance.set" #typeWord, handlerModuleClassSet##typeWord),    \
            "LL" #typeChar,                                                                        \
            "");                                                                                   \
    registerFuncProvider(                                                                          \
            FuncProvider("core.class.instance.get" #typeWord, handlerModuleClassGet##typeWord),    \
            "LL",                                                                                  \
            #typeChar)

ModuleWorker& getCurrentModuleWorker()
{
    return moduleManager.getModuleWorkerByThreadId(std::this_thread::get_id());
}

FuncResult handlerRegisterModuleFuncProvider(const std::vector<void*>& args)
{
    if (args.size() != 3) {
        throw std::logic_error(
                "Invalid number of arguments for handlerRegisterModuleFuncProvider()");
    }
    FuncResult result;
    result.data.resize(1);

    const std::string& fpname = getArgument<std::string>(args, 0);
    std::string fpargs = getArgument<std::string>(args, 1);
    std::string fpret = getArgument<std::string>(args, 2);

    ModuleWorker& worker = getCurrentModuleWorker();
    auto pair = worker.registerModuleFuncProvider(fpname, fpargs, fpret);
    registerFuncProvider(FuncProvider(fpname, pair.second), fpargs, fpret);
    setReturn(result, 0, pair.first);
    return result;
}

FuncResult handlerGetModuleClassHandle(const std::vector<void*>& args)
{
    if (args.size() != 1) {
        throw std::logic_error("Invalid number of arguments for handlerGetModuleClassHandle()");
    }
    FuncResult result;
    result.data.resize(1);

    const std::string& name = getArgument<std::string>(args, 0);
    try {
        setReturn<uint64_t>(result, 0, moduleClassHandles.at(name));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error(std::string("No such class name: '") + name + "'");
    }

    return result;
}
FuncResult handlerGetModuleClassMemberHandle(const std::vector<void*>& args)
{
    if (args.size() != 2) {
        throw std::logic_error("Invalid number of arguments for handlerGetModuleClassHandle()");
    }
    FuncResult result;
    result.data.resize(1);

    uint64_t handle = getArgument<uint64_t>(args, 0);
    const std::string& name = getArgument<std::string>(args, 1);
    try {
        setReturn<uint64_t>(result, 0, moduleClasses.access(handle).memberHandles.at(name));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error(std::string("No such member of class ") + std::to_string(handle)
                                 + ": '" + name + "'");
    }

    return result;
}
FuncResult handlerGetModuleClassMethod(const std::vector<void*>& args)
{
    if (args.size() != 2) {
        throw std::logic_error("Invalid number of arguments for handlerGetModuleClassMethod()");
    }
    FuncResult result;
    result.data.resize(1);

    uint64_t handle = getArgument<uint64_t>(args, 0);
    const std::string& name = getArgument<std::string>(args, 1);
    try {
        setReturn<std::string>(
                result,
                0,
                moduleClasses.access(handle)
                        .methods.at(moduleClasses.access(handle).methodHandles.at(name))
                        .name);
    } catch (const std::out_of_range& e) {
        throw std::runtime_error(std::string("No such method of class ") + std::to_string(handle)
                                 + ": '" + name + "'");
    }

    return result;
}

static void initializeCoreFuncProviders()
{
    registerFuncProvider(FuncProvider("core.class.add", handlerAddModuleClass), "Lsss", "L");
    registerFuncProvider(
            FuncProvider("core.class.instantiate", handlerInstantiateModuleClass), "L", "L");
    registerFuncProvider(
            FuncProvider("core.funcProvider.register", handlerRegisterModuleFuncProvider),
            "sss",
            "L");
    registerFuncProvider(
            FuncProvider("core.class.getHandle", handlerGetModuleClassHandle), "s", "L");
    registerFuncProvider(
            FuncProvider("core.class.getMemberHandle", handlerGetModuleClassMemberHandle),
            "Ls",
            "L");
    registerFuncProvider(
            FuncProvider("core.class.getMethod", handlerGetModuleClassMethod), "Ls", "s");
    HANDLER_MODCLASS_ACCESSOR_REGISTER(String, s);
    HANDLER_MODCLASS_ACCESSOR_REGISTER(Int8, b);
    HANDLER_MODCLASS_ACCESSOR_REGISTER(Int16, h);
    HANDLER_MODCLASS_ACCESSOR_REGISTER(Int32, i);
    HANDLER_MODCLASS_ACCESSOR_REGISTER(Int64, l);
    HANDLER_MODCLASS_ACCESSOR_REGISTER(UInt8, B);
    HANDLER_MODCLASS_ACCESSOR_REGISTER(UInt16, H);
    HANDLER_MODCLASS_ACCESSOR_REGISTER(UInt32, I);
    HANDLER_MODCLASS_ACCESSOR_REGISTER(UInt64, L);
    HANDLER_MODCLASS_ACCESSOR_REGISTER(Float32, f);
    HANDLER_MODCLASS_ACCESSOR_REGISTER(Float64, F);
    HANDLER_MODCLASS_ACCESSOR_REGISTER(WideString, w);
    HANDLER_MODCLASS_ACCESSOR_REGISTER(Blob, o);
}

#undef HANDLER_MODCLASS_ACCESSOR_REGISTER
