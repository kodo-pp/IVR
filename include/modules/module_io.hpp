#ifndef MODULES_MODULE_IO_HPP
#define MODULES_MODULE_IO_HPP

#include <cassert>
#include <string>
#include <type_traits>

#include <modbox/core/core.hpp>
#include <modbox/core/dyntype.hpp>
#include <modbox/core/memory_manager.hpp>
#include <modbox/log/log.hpp>

void readModuleHeader(int sock);
void readReverseModuleHeader(int sock);
std::string readModuleName(int sock);

template <typename T>
T getArgument(const std::vector<std::string>& args, size_t idx)
{
    return DyntypeCaster<T>::get(args.at(idx));
}

template <typename T>
void setReturn(FuncResult& res, size_t idx, const T& value)
{
    res.data.at(idx) = DyntypeCaster<std::string>::get(value);
}

#endif /* end of include guard: MODULES_MODULE_IO_HPP */
