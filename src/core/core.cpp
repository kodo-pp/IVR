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
#include <boost/lexical_cast.hpp>

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
FuncResult FuncProvider::operator()(const std::vector<void*>& args) const
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

ModuleClassMemberData::ModuleClassMemberData(const ModuleClassMemberData& other)
        : type(other.type), value(other.value), referenceCount(other.referenceCount)
{
    (*referenceCount)++;
}
ModuleClassMemberData::~ModuleClassMemberData()
{
    (*referenceCount)--;
    if (*referenceCount <= 0) {
        dyntypeDelete(value, type);
    }
}

ModuleClassMemberData& ModuleClassMemberData::operator=(const ModuleClassMemberData& other)
{
    type = other.type;
    value = other.value;
    referenceCount = other.referenceCount;
    (*referenceCount)++;
    return *this;
}

ModuleClass::ModuleClass(const std::unordered_map<std::string, ModuleClassMember>& _members,
                         const std::unordered_map<std::string, ModuleClassMethod>& _methods,
                         const std::string& parent = "")
        : parentClassName(parent), members(_members), methods(_methods)
{
    if (!parent.empty()) {
        const auto parentClass = getModuleClass(parent);
        members.insert(parentClass.members.begin(), parentClass.members.end());
        methods.insert(parentClass.methods.begin(), parentClass.methods.end());
    }
}

ModuleClassInstance::ModuleClassInstance(const std::string& _className) : className(_className)
{
    const auto& memberDecls = getModuleClass(className).members;
    members.reserve(memberDecls.size());
    for (const auto& kv : memberDecls) {
        members.insert({kv.first, ModuleClassMemberData(kv.second.type)});
    }
}

std::recursive_mutex moduleClassMutex;

static std::unordered_map<std::string, ModuleClass> moduleClasses;
static HandleStorage<uint64_t, ModuleClassInstance> moduleClassInstances;

void addModuleClass(const std::string& name, const ModuleClass& moduleClass)
{
    std::lock_guard<std::recursive_mutex> lock(moduleClassMutex);
    moduleClasses.insert({name, moduleClass});
}

void removeModuleClass(const std::string& name)
{
    std::lock_guard<std::recursive_mutex> lock(moduleClassMutex);
    moduleClasses.erase(name);
}

const ModuleClass& getModuleClass(const std::string& className)
{
    std::lock_guard<std::recursive_mutex> lock(moduleClassMutex);
    return moduleClasses.at(className);
}

uint64_t instantiateModuleClass(const std::string& className)
{
    std::lock_guard<std::recursive_mutex> lock(moduleClassMutex);
    return moduleClassInstances.insert(ModuleClassInstance(className));
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
    std::string parent = getArgument<std::string>(args, 0);
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

    addModuleClass(name, ModuleClass(memberMap, methodMap, parent));

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
    std::string className = getArgument<std::string>(args, 0);
    auto handle = instantiateModuleClass(className);
    setReturn<uint64_t>(ret, 0, handle);

    return ret;
}

FuncResult handlerGetModuleClassMethod(const std::vector<void*>& args)
{
    if (args.size() != 2) {
        throw std::logic_error("Wrong number of arguments for handlerGetModuleClassMethod()");
    }
    FuncResult ret;
    ret.data.resize(3);

    std::string className = getArgument<std::string>(args, 0);
    std::string methodName = getArgument<std::string>(args, 1);

    const auto& method = getModuleClass(className).methods.at(methodName);
    std::string funcProviderName = method.name;
    std::string argTypes = method.arguments_type;
    std::string retTypes = method.return_type;
    setReturn(ret, 0, funcProviderName);
    setReturn(ret, 1, argTypes);
    setReturn(ret, 2, retTypes);
    return ret;
}

#define HANDLER_MODCLASS_ACCESSOR(typeWord, typeNameCxx, typeChar)                                   \
    FuncResult handlerModuleClassSet##typeWord(const std::vector<void*>& args)                       \
    {                                                                                                \
        if (args.size() != 3) {                                                                      \
            throw std::logic_error("Wrong number of arguments for handlerModuleClassSet" #typeWord   \
                                   "()");                                                            \
        }                                                                                            \
        FuncResult ret;                                                                              \
        auto instanceHandle = getArgument<uint64_t>(args, 0);                                        \
        auto memberName = getArgument<std::string>(args, 1);                                         \
        auto value = getArgument<typeNameCxx>(args, 2);                                              \
                                                                                                     \
        moduleClassInstances.mutableAccess(instanceHandle).members.at(memberName).get<typeNameCxx>() \
                = value;                                                                             \
        return ret;                                                                                  \
    }                                                                                                \
                                                                                                     \
    FuncResult handlerModuleClassGet##typeWord(const std::vector<void*>& args)                       \
    {                                                                                                \
        if (args.size() != 2) {                                                                      \
            throw std::logic_error("Wrong number of arguments for handlerModuleClassGet" #typeWord   \
                                   "()");                                                            \
        }                                                                                            \
        FuncResult ret;                                                                              \
        ret.data.resize(1);                                                                          \
        auto instanceHandle = getArgument<uint64_t>(args, 0);                                        \
        auto memberName = getArgument<std::string>(args, 1);                                         \
                                                                                                     \
        typeNameCxx value = moduleClassInstances.access(instanceHandle)                              \
                                    .members.at(memberName)                                          \
                                    .get<typeNameCxx>();                                             \
                                                                                                     \
        setReturn<typeNameCxx>(ret, 0, value);                                                       \
        return ret;                                                                                  \
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
HANDLER_MODCLASS_ACCESSOR(Blob, std::vector<uint8_t>, 'o')
#undef HANDLER_MODCLASS_ACCESSOR

#define HANDLER_MODCLASS_ACCESSOR_REGISTER(typeWord, typeChar)                                     \
    registerFuncProvider(                                                                          \
            FuncProvider("core.class.instance.set" #typeWord, handlerModuleClassSet##typeWord),    \
            "Ls" #typeChar,                                                                        \
            "");                                                                                   \
    registerFuncProvider(                                                                          \
            FuncProvider("core.class.instance.get" #typeWord, handlerModuleClassGet##typeWord),    \
            "Ls",                                                                                  \
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

std::vector<uint8_t> moduleClassBlobifyMembers(uint64_t objectHandle)
{
    std::vector<uint8_t> blob;
    const auto& instance = getModuleClassInstance(objectHandle);
    for (const auto& kv : instance.members) {
        blob.reserve(blob.size() + kv.first.length() + 2);
        for (char c : kv.first) {
            blob.push_back(static_cast<uint8_t>(c));
        }
        blob.push_back(static_cast<uint8_t>(kv.second.type));
        blob.push_back(0);
        std::string enc;
        switch (kv.second.type) {
        case 'b':
            enc = std::to_string(kv.second.get<int8_t>());
            break;
        case 'B':
            enc = std::to_string(kv.second.get<uint8_t>());
            break;
        case 'h':
            enc = std::to_string(kv.second.get<int16_t>());
            break;
        case 'H':
            enc = std::to_string(kv.second.get<uint16_t>());
            break;
        case 'i':
            enc = std::to_string(kv.second.get<int32_t>());
            break;
        case 'I':
            enc = std::to_string(kv.second.get<uint32_t>());
            break;
        case 'l':
            enc = std::to_string(kv.second.get<int64_t>());
            break;
        case 'L':
            enc = std::to_string(kv.second.get<uint64_t>());
            break;
        case 's':
            enc = kv.second.get<std::string>();
            break;
        case 'w':
            enc = bytes_pack(kv.second.get<std::wstring>());
            break;
        case 'o':
            throw std::runtime_error("Blob as member is not implemented");
        default:
            throw std::logic_error("blobify: unknown type");
        }
        blob.reserve(blob.size() + enc.length() + 1);
        for (char c : enc) {
            blob.push_back(static_cast<uint8_t>(c));
        }
        blob.push_back(0);
    }

    return blob;
}

void moduleClassUnblobifyMembers(uint64_t objectHandle, const std::vector<uint8_t>& blob)
{
    ModuleClassInstance& instance = getModuleClassInstance(objectHandle);
    size_t idx = 0;
    try {
        while (true) {
            if (idx == blob.size()) {
                break;
            }
            std::string namet, enc;
            for (;; ++idx) {
                auto val = blob.at(idx);
                if (val == 0) {
                    break;
                }
                namet.push_back(val);
            }
            for (;; ++idx) {
                auto val = blob.at(idx);
                if (val == 0) {
                    break;
                }
                enc.push_back(val);
            }
            std::string name = namet;
            name.pop_back();
            char type = namet.back();
            switch (type) {
            case 'b':
                instance.members.at(name).set(boost::lexical_cast<int8_t>(enc));
                break;
            case 'B':
                instance.members.at(name).set(boost::lexical_cast<uint8_t>(enc));
                break;
            case 'h':
                instance.members.at(name).set(boost::lexical_cast<int16_t>(enc));
                break;
            case 'H':
                instance.members.at(name).set(boost::lexical_cast<uint16_t>(enc));
                break;
            case 'i':
                instance.members.at(name).set(boost::lexical_cast<int32_t>(enc));
                break;
            case 'I':
                instance.members.at(name).set(boost::lexical_cast<uint32_t>(enc));
                break;
            case 'l':
                instance.members.at(name).set(boost::lexical_cast<int64_t>(enc));
                break;
            case 'L':
                instance.members.at(name).set(boost::lexical_cast<uint64_t>(enc));
                break;
            case 's':
                instance.members.at(name).set<std::string>(enc);
                break;
            case 'w':
                instance.members.at(name).set<std::wstring>(wstringUnpack(enc));
                break;
            case 'o':
                throw std::runtime_error("Blob as member is not implemented");
            default:
                throw std::logic_error("blobify: unknown type");
            }
        }
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("Malformed blobified member diff");
    }
}

std::string moduleClassBindMethod(std::string className, std::string methodName)
{
    static uint64_t counter = 1;
    std::string funcName = "core.class.__bound__.";
    funcName += std::to_string(counter);
    auto method = getModuleClass(className).methods.at(methodName);

    if (method.arguments_type.substr(0, 2) != "Lo") {
        throw std::runtime_error("Method cannot be bound: invalid arguments type");
    }
    method.arguments_type.erase(1, 1);
    if (method.return_type.substr(0, 1) != "o") {
        throw std::runtime_error("Method cannot be bound: invalid return type");
    }
    method.return_type.erase(0, 1);
    std::string command = method.name;

    auto& funcProvider = getFuncProvider(getFuncProviderHandle(command));

    registerFuncProvider(
            FuncProvider(
                    funcName,
                    [className, methodName, funcProvider](std::vector<void*> args) -> FuncResult {
                        auto* blob = static_cast<std::vector<uint8_t>*>(dyntypeNew('o'));
                        uint64_t objectHandle = getArgument<uint64_t>(args, 0);
                        auto encoded = moduleClassBlobifyMembers(objectHandle);
                        blob->swap(encoded);
                        args.emplace(args.begin() + 1, static_cast<void*>(blob));
                        FuncResult res = funcProvider(args);
                        dyntypeDelete(blob, 'o');
                        auto retBlob = static_cast<std::vector<uint8_t>*>(res.data.front());
                        moduleClassUnblobifyMembers(objectHandle, *retBlob);
                        dyntypeDelete(retBlob, 'o');
                        res.data.erase(res.data.begin());
                        return res;
                    }),
            method.arguments_type,
            method.return_type);
    ++counter;
    return funcName;
}

ModuleClassInstance& getModuleClassInstance(uint64_t handle)
{
    return moduleClassInstances.mutableAccess(handle);
}

static void initializeCoreFuncProviders()
{
    registerFuncProvider(FuncProvider("core.class.add", handlerAddModuleClass), "ssss", "");
    registerFuncProvider(
            FuncProvider("core.class.instantiate", handlerInstantiateModuleClass), "s", "L");
    registerFuncProvider(
            FuncProvider("core.funcProvider.register", handlerRegisterModuleFuncProvider),
            "sss",
            "L");
    registerFuncProvider(
            FuncProvider("core.class.getMethod", handlerGetModuleClassMethod), "ss", "sss");
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
