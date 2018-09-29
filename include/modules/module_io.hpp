#ifndef MODULES_MODULE_IO_HPP
#define MODULES_MODULE_IO_HPP

#include <cassert>
#include <string>
#include <type_traits>

#include <modbox/core/core.hpp>
#include <modbox/core/memory_manager.hpp>
#include <modbox/log/log.hpp>

void readModuleHeader(int sock);
void readReverseModuleHeader(int sock);
std::wstring readModuleName(int sock);

void* recvArg(int sock, char spec);
void sendArg(int sock, void* arg, char spec);
void freeArg(void* arg, char spec);

template <typename T>
T getArgument(const std::vector<void*>& args, size_t idx)
{
    assert(args.at(idx) != nullptr);
    return *static_cast<T*>(args[idx]);
}

template <typename T>
void setReturn(FuncResult& res, size_t idx, const T& value)
{
    // TODO: VERY uGly
#ifdef FORTIFY_SOURCE
    res.data.at(idx)
#else
    res.data[idx]
#endif
            = static_cast<void*>(new T(value));
    memoryManager.track(res.data.at(idx));
}

#endif /* end of include guard: MODULES_MODULE_IO_HPP */
