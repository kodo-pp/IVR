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

#include <modbox/core/core.hpp>
#include <modbox/core/dyntype.hpp>
#include <modbox/log/log.hpp>
#include <modbox/misc/die.hpp>
#include <modbox/modules/module_io.hpp>
#include <modbox/modules/module_manager.hpp>
#include <modbox/util/handle_storage.hpp>
#include <modbox/util/util.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

// === Static variables ===

// Map: command -> handle
std::unordered_map<std::string, std::tuple<FuncProvider, ArgsSpec, ArgsSpec>> funcProviderMap;

// Mutex protecting the access to FuncProvider functions
static std::recursive_mutex funcProviderMutex;

// === Implementation of FuncProvider methods ===

// Default constructor
FuncProvider::FuncProvider() = default;

// AWW, this codestyle is awful. TODO: make it look beautiful
// Command-and-function constructor
FuncProvider::FuncProvider(
        const std::string& _command,
        const std::function<struct FuncResult(const std::vector<std::string>&)>& _func)
        : command(_command), func(_func)
{
}

// Destructor
FuncProvider::~FuncProvider()
{
}

// operator()
FuncResult FuncProvider::operator()(const std::vector<std::string>& args) const
{
    return func(args);
}

std::string FuncProvider::getCommand() const
{
    return command;
}

// === Initialization functions ===

static void initializeCoreFuncProviders();

void initilaizeCore(UNUSED std::vector<std::string>& args)
{
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
    funcProviderMap.insert({command, {prov, argsSpec, retSpec}});
    LOG(L"Successfully registered FuncProvider for " << wstring_cast(command));
}

const FuncProvider& getFuncProvider(const std::string& command)
{
    std::lock_guard<std::recursive_mutex> lock(funcProviderMutex);
    try {
        return std::get<0>(funcProviderMap.at(command));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No such FuncProvider: " + command);
    }
}

ArgsSpec getArgsSpec(const std::string& command)
{
    std::lock_guard<std::recursive_mutex> lock(funcProviderMutex);
    try {
        return std::get<1>(funcProviderMap.at(command));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No such FuncProvider: " + command);
    }
}

ArgsSpec getRetSpec(const std::string& command)
{
    std::lock_guard<std::recursive_mutex> lock(funcProviderMutex);
    try {
        return std::get<2>(funcProviderMap.at(command));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("No such FuncProvider: " + command);
    }
}

void funcProvidersCleanup()
{
}

ModuleClass::ModuleClass(const std::unordered_map<std::string, ModuleClassMember>& _members,
                         const std::unordered_map<std::string, ModuleClassMethod>& _methods,
                         const std::string& className,
                         const std::string& parent)
        : parentClassName(parent), members(_members), methods(_methods)
{
    if (!parent.empty()) {
        const auto& parentClass = getModuleClass(parent);
        members.insert(parentClass.members.begin(), parentClass.members.end());
        methods.insert(parentClass.methods.begin(), parentClass.methods.end());
    }
    for (auto& kv : methods) {
        auto& method = kv.second;
        std::tie(method.name, method.arguments_type, method.return_type) = moduleClassBindMethod(
                className, method.name, method.arguments_type, method.return_type);
    }
}

ModuleClassInstance::ModuleClassInstance(const std::string& _className) : className(_className)
{
    const auto& memberDecls = getModuleClass(className).members;
    members.reserve(memberDecls.size());
    for (const auto& kv : memberDecls) {
        members.insert({kv.first, ModuleClassMemberData(std::string(1, kv.second.type))});
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
        std::string enc = kv.second.get<std::string>();

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
                    ++idx;
                    break;
                }
                namet.push_back(val);
            }
            for (;; ++idx) {
                auto val = blob.at(idx);
                if (val == 0) {
                    ++idx;
                    break;
                }
                enc.push_back(val);
            }
            std::string name = namet;
            name.pop_back();
            instance.members.at(name).set(enc);
        }
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("Malformed blobified member diff");
    }
}

std::tuple<std::string, std::string, std::string> moduleClassBindMethod(const std::string& className,
                                                                        const std::string& command,
                                                                        std::string argTypes,
                                                                        std::string retTypes)
{
    LOG("bind method: classname '" << className << "', command '" << command << "', args '"
                                   << argTypes << "', rets '" << retTypes << "'");
    std::lock_guard<std::recursive_mutex> lock(moduleClassMutex);
    static uint64_t counter = 1;
    std::string funcName = "core.class.__bound__.";
    funcName += std::to_string(counter);

    if (argTypes.substr(0, 2) != "ub") {
        throw std::runtime_error("Method cannot be bound: invalid arguments type");
    }
    argTypes.erase(1, 1);
    if (retTypes.substr(0, 1) != "b") {
        throw std::runtime_error("Method cannot be bound: invalid return type");
    }
    retTypes.erase(0, 1);

    auto& funcProvider = getFuncProvider(command);

    registerFuncProvider(
            FuncProvider(
                    funcName,
                    [className, funcProvider](std::vector<std::string> args) -> FuncResult {
                        try {
                            // Get data
                            uint64_t objectHandle = getArgument<uint64_t>(args, 0);
                            std::vector<uint8_t> blob = moduleClassBlobifyMembers(objectHandle);

                            // Add blob to arguments
                            args.emplace(args.begin() + 1, DyntypeCaster<std::string>::get(blob));

                            // Call FuncProvider
                            FuncResult res = funcProvider(args);

                            // Get return blob
                            auto retBlob = DyntypeCaster<std::vector<uint8_t>>::get(
                                    res.data.front());

                            std::cerr << std::endl;

                            // Unpack it
                            moduleClassUnblobifyMembers(objectHandle, retBlob);

                            // And remove it from return vector
                            res.data.erase(res.data.begin());
                            return res;
                        } catch (const std::exception& e) {
                            LOG("Exception happend at binding lambda: " << wstring_cast(e.what()));
                            logStackTrace();
                            throw e;
                        }
                    }),
            argTypes,
            retTypes);
    ++counter;
    return {funcName, argTypes, retTypes};
}

FuncResult handlerAddModuleClass(const std::vector<std::string>& args)
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

    addModuleClass(name, ModuleClass(memberMap, methodMap, name, parent));

    return ret;
}

// void handlerRemoveModuleClass(const std::string& name);
FuncResult handlerInstantiateModuleClass(const std::vector<std::string>& args)
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

FuncResult handlerGetModuleClassMethod(const std::vector<std::string>& args)
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

FuncResult handlerModuleClassSet(const std::vector<std::string>& args)
{
    if (args.size() != 3) {
        throw std::logic_error("Wrong number of arguments for handlerModuleClassSet()");
    }
    FuncResult ret;
    auto instanceHandle = getArgument<uint64_t>(args, 0);
    auto memberName = getArgument<std::string>(args, 1);
    auto value = getArgument<std::string>(args, 2);

    auto& tmp = moduleClassInstances.mutableAccess(instanceHandle);
    auto& tmp2 = tmp.members.at(memberName);
    tmp2.genericSet(value);
    return ret;
}

FuncResult handlerModuleClassGet(const std::vector<std::string>& args)
{
    if (args.size() != 2) {
        throw std::logic_error("Wrong number of arguments for handlerModuleClassGet()");
    }
    FuncResult ret;
    ret.data.resize(1);
    auto instanceHandle = getArgument<uint64_t>(args, 0);
    auto memberName = getArgument<std::string>(args, 1);

    std::string value
            = moduleClassInstances.access(instanceHandle).members.at(memberName).get<std::string>();

    setReturn(ret, 0, value);
    return ret;
}

ModuleWorker& getCurrentModuleWorker()
{
    return moduleManager.getModuleWorkerByThreadId(std::this_thread::get_id());
}

FuncResult handlerRegisterModuleFuncProvider(const std::vector<std::string>& args)
{
    if (args.size() != 3) {
        throw std::logic_error(
                "Invalid number of arguments for handlerRegisterModuleFuncProvider()");
    }
    FuncResult result;

    const std::string& fpname = getArgument<std::string>(args, 0);
    std::string fpargs = getArgument<std::string>(args, 1);
    std::string fpret = getArgument<std::string>(args, 2);

    ModuleWorker& worker = getCurrentModuleWorker();
    auto func = worker.registerModuleFuncProvider(fpname, fpargs, fpret);
    registerFuncProvider(FuncProvider(fpname, func), fpargs, fpret);
    return result;
}

ModuleClassInstance& getModuleClassInstance(uint64_t handle)
{
    return moduleClassInstances.mutableAccess(handle);
}

FuncResult handlerClassNop(UNUSED const std::vector<std::string>& args)
{
    FuncResult res;
    res.data.resize(1);
    setReturn(res, 0, std::vector<uint8_t>());
    return res;
}

static void initializeCoreFuncProviders()
{
    registerFuncProvider(FuncProvider("core.class.add", handlerAddModuleClass), "ssss", "");
    registerFuncProvider(
            FuncProvider("core.class.instantiate", handlerInstantiateModuleClass), "s", "u");
    registerFuncProvider(
            FuncProvider("core.funcProvider.register", handlerRegisterModuleFuncProvider),
            "sss",
            "");
    registerFuncProvider(
            FuncProvider("core.class.getMethod", handlerGetModuleClassMethod), "ss", "sss");
    registerFuncProvider(FuncProvider("core.class.nop", handlerClassNop), "ub", "b");

    registerFuncProvider(FuncProvider("core.class.instance.set", handlerModuleClassSet), "uss", "");
    registerFuncProvider(FuncProvider("core.class.instance.get", handlerModuleClassGet), "us", "s");
}

void ModuleClassMemberData::genericSet(const std::string& x)
{
    LOG("ModuleClassMemberData::genericSet(" << x << ") @ '" << type << "'");
    value = x;
}
