#ifndef UTIL_DSO_HPP
#define UTIL_DSO_HPP

#include <functional>
#include <string>

#include <modbox/log/log.hpp>
#include <modbox/util/util.hpp>

#include <dlfcn.h>

class Dso
{
public:
    Dso(const std::string& _filename);
    Dso(const Dso& other) = delete;
    Dso(Dso&& other) = default;
    virtual ~Dso();

    template <typename Ret, typename... Args>
    std::function<Ret(Args...)> resolve(const std::string& symbolName) const
    {
        LOG("Resolve");
        auto voidptr = dlsym(dso_handle, symbolName.c_str());
        LOG("voidptr = " << voidptr);
        auto ptr = reinterpret_cast<Ret (*)(Args...)>(voidptr);
        LOG("ptr = " << ptr);
        if (ptr == nullptr) {
            LOG("No such dynamic symbol: " << wstring_cast(symbolName) << ": " << dlerror());
            throw std::runtime_error("No such dynamic symbol: " + symbolName);
        }
        LOG("End resolve");
        return ptr;
    }

    Dso& operator=(const Dso& other) = delete;
    Dso& operator=(Dso&& other) = default;

    std::string getFilename() const;

private:
    std::string filename;
    void* dso_handle;
};

#endif /* end of include guard: UTIL_DSO_HPP */
