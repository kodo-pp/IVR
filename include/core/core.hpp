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
    std::vector<void*> data;
};

// === FuncProvider definition ===

class FuncProvider
{
public:
    using func_type = std::function<FuncResult(const std::vector<void*>&)>;

    FuncProvider();
    FuncProvider(const std::string&, const func_type&);
    std::string getCommand() const;
    FuncResult operator()(const std::vector<void*>&) const;
    ~FuncProvider();

private:
    std::string command;
    func_type func;
};

// === Initialization function ===

void initilaizeCore(std::vector<std::string>&);

// === Working with FuncProviders ===

void registerFuncProvider(const FuncProvider&, ArgsSpec, ArgsSpec);

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

TYPECHAR(int8_t, 'b');
TYPECHAR(int16_t, 'h');
TYPECHAR(int32_t, 'i');
TYPECHAR(int64_t, 'l');
TYPECHAR(uint8_t, 'B');
TYPECHAR(uint16_t, 'H');
TYPECHAR(uint32_t, 'I');
TYPECHAR(uint64_t, 'L');
TYPECHAR(float, 'f');
TYPECHAR(double, 'F');
TYPECHAR(std::string, 's');
// TYPECHAR(std::string, 'o'); // TODO: blob
TYPECHAR(std::wstring, 'w');

#undef TYPECHAR

struct ModuleClassMemberData
{
    template <typename T>
    explicit ModuleClassMemberData(T* val) : referenceCount(std::make_shared<size_t>(1))
    {
        type = TypeChar<T>::value;
        if (type == '?') {
            throw std::runtime_error("Unknown type");
        }
        value = val;
    }
    template <typename T>
    explicit ModuleClassMemberData(const T& val) : referenceCount(std::make_shared<size_t>(1))
    {
        type = TypeChar<T>::value;
        if (type == '?') {
            throw std::runtime_error("Unknown type");
        }
        value = new T(val);
    }
    explicit ModuleClassMemberData(char _type) : referenceCount(std::make_shared<size_t>(1))
    {
        type = _type;
        if (type == '?') {
            throw std::runtime_error("Unknown type");
        }
        value = dyntypeNew(type);
    }

    // TODO: Написать move-конструктор и оператор присваивания
    // Пока они =delete, т.к. дефолтные приводят к Segfault

    ModuleClassMemberData(const ModuleClassMemberData& other);
    ModuleClassMemberData(ModuleClassMemberData&& other) = delete;
    virtual ~ModuleClassMemberData();

    ModuleClassMemberData& operator=(const ModuleClassMemberData& other);
    ModuleClassMemberData& operator=(ModuleClassMemberData&& other) = delete;

    char type;
    void* value;
    template <typename T>
    T& get()
    {
        if (type != TypeChar<T>::value) {
            logStackTrace();
            throw std::runtime_error(std::string("Type mismatch(get): expected ")
                                     + TypeChar<T>::value + ", got " + type);
        } else {
            return *static_cast<T*>(value);
        }
    }
    template <typename T>
    const T& get() const
    {
        if (type != TypeChar<T>::value) {
            logStackTrace();
            throw std::runtime_error(std::string("Type mismatch(get const): expected ")
                                     + TypeChar<T>::value + ", got " + type);
        } else {
            return *static_cast<T*>(value);
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
            *static_cast<T*>(value) = x;
        }
    }

    mutable std::shared_ptr<size_t> referenceCount;
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
