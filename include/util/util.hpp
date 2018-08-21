#ifndef UTIL_UTIL_HPP
#define UTIL_UTIL_HPP

#include <atomic>
#include <cmath>
#include <string>
#include <thread>
#include <type_traits>

#include <unistd.h>

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#define NORETURN __attribute__((noreturn))
#define PACKED __attribute__((packed))
#else
#define UNUSED
#define NORETURN
#define PACKED __attribute__((packed))
#endif

std::wstring wstring_cast(const std::string& s);

std::string bytes_pack(const std::string& s);
std::string bytes_pack(const std::wstring& ws);

std::wstring wstringUnpack(const std::string& bytes);

template <typename T>
void delayedAssign(std::atomic<T>& var, double sec, const T& value)
{
    std::thread thr([&var, sec, value]() {
        usleep(round(sec * 1e+6));
        var = value;
    });
    thr.detach();
}

template <typename T>
T intFlipEndian(T n)
{
    static_assert(std::is_integral<T>::value, "intFlipEndian: argument is not integer");
    T tmp = 0;
    for (size_t i = 0; i < sizeof(n); ++i) {
        tmp |= ((n >> (i * 8)) & 0xFF) << ((sizeof(n) - i - 1) * 8);
    }
    return tmp;
}

#endif /* end of include guard: UTIL_UTIL_HPP */
