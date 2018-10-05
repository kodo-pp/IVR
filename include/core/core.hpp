#ifndef CORE_CORE_HPP
#define CORE_CORE_HPP

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <modbox/core/dyntype.hpp>
#include <modbox/util/util.hpp>

// === Type definitions ===

using ArgsSpec = std::string;

struct FuncResult
{
    std::vector<std::string> data;
};

// === FuncProvider definition ===

class FuncProvider
{
public:
    using func_type = std::function<FuncResult(const std::vector<std::string>&)>;

    FuncProvider();
    FuncProvider(const std::string& name, const func_type& args);
    std::string getCommand() const;
    FuncResult operator()(const std::vector<std::string>& args) const;
    ~FuncProvider();

private:
    std::string command;
    func_type func;
};

// === Initialization function ===

void initilaizeCore(std::vector<std::string>& args);

// === Working with FuncProviders ===

void registerFuncProvider(const FuncProvider& provider, ArgsSpec args, ArgsSpec ret);
uint64_t getFuncProviderHandle(const std::string& command);
const FuncProvider& getFuncProvider(uint64_t handle);
ArgsSpec getArgsSpec(uint64_t handle);
ArgsSpec getRetSpec(uint64_t handle);

void funcProvidersCleanup();

struct ModuleClassMember
{
    char type;
};

struct ModuleClassMethod
{
    std::string name;
    std::string arguments_type;
    std::string return_type;
};

struct ModuleClass
{
    ModuleClass(const std::unordered_map<std::string, ModuleClassMember>& _members,
                const std::unordered_map<std::string, ModuleClassMethod>& _methods,
                const std::string& className,
                const std::string& parent);

    std::string parentClassName;
    std::unordered_map<std::string, ModuleClassMember> members;
    std::unordered_map<std::string, ModuleClassMethod> methods;
    std::unordered_map<std::string, std::string> boundMethods;
};

template <typename T>
struct TypeChar
{
    static constexpr char value = '?';
};

#define TYPECHAR(type, c)                                                                          \
    template <>                                                                                    \
    struct TypeChar<type>                                                                          \
    {                                                                                              \
        static constexpr char value = c;                                                           \
    }

TYPECHAR(int64_t, 'i');
TYPECHAR(uint64_t, 'u');
TYPECHAR(double, 'f');
TYPECHAR(std::string, 's');
TYPECHAR(std::vector<uint8_t>, 'b');

#undef TYPECHAR

struct ModuleClassMemberData
{
    template <typename T>
    explicit ModuleClassMemberData(const T& val)
    {
        value = DyntypeCaster<std::string>::get(val);
        type = TypeChar<T>::value;
    }

    ModuleClassMemberData(const ModuleClassMemberData& other) = default;
    ModuleClassMemberData(ModuleClassMemberData&& other) = default;
    virtual ~ModuleClassMemberData();

    ModuleClassMemberData& operator=(const ModuleClassMemberData& other);
    ModuleClassMemberData& operator=(ModuleClassMemberData&& other) = delete;

    char type;
    std::string value;
    template <typename T>
    const T get() const
    {
        if (type != TypeChar<T>::value) {
            logStackTrace();
            throw std::runtime_error(std::string("Type mismatch(get const): expected ")
                                     + TypeChar<T>::value + ", got " + type);
        } else {
            return DyntypeCaster<T>::get(value);
        }
    }
    template <typename T>
    void set(const T& x)
    {
        if (type != TypeChar<T>::value) {
            logStackTrace();
            throw std::runtime_error(std::string("Type mismatch(set): expected ")
                                     + TypeChar<T>::value + ", got " + type);
        } else {
            value = DyntypeCaster<std::string>::get(x);
        }
    }
};

struct ModuleClassInstance
{
    explicit ModuleClassInstance(const std::string& _className);
    ModuleClassInstance(const ModuleClassInstance& other) = default;
    ModuleClassInstance(ModuleClassInstance&& other) = default;
    ~ModuleClassInstance() = default;

    std::string className;

    std::unordered_map<std::string, ModuleClassMemberData> members;
};

void addModuleClass(const std::string& name, const ModuleClass& moduleClass);
void removeModuleClass(const std::string& name);
const ModuleClass& getModuleClass(const std::string& className);
uint64_t instantiateModuleClass(const std::string& className);
ModuleClassInstance& getModuleClassInstance(uint64_t instanceId);
void deleteModuleClassInstance(uint64_t instanceId);

std::tuple<std::string, std::string, std::string> moduleClassBindMethod(const std::string& className,
                                                                        const std::string& command,
                                                                        std::string argTypes,
                                                                        std::string retTypes);

// === Constants ===
const uint64_t RESERVED_FP_HANDLE = 0xFFFF'FFFF'4E5E'47EDull;

#endif /* end of include guard: CORE_CORE_HPP */
