#ifndef UTIL_UTIL_HPP
#define UTIL_UTIL_HPP

#include <string>
#include <type_traits>
std::wstring wstring_cast(const std::string& s);

std::string bytes_pack(const std::string& s);
std::string bytes_pack(const std::wstring& ws);

std::wstring wstringUnpack(const std::string& bytes);

template <typename T>
T intFlipEndian(T n) {
    static_assert(std::is_integral<T>::value, "intFlipEndian: argument is not integer");
    T tmp = 0;
    for (size_t i = 0; i < sizeof(n); ++i) {
        tmp |= ((n >> (i * 8)) & 0xFF) << ((sizeof(n) - i - 1) * 8);
    }
    return tmp;
}

#endif /* end of include guard: UTIL_UTIL_HPP */