#include <string>

#include <modbox/util/util.hpp>

// TODO: add UTF-8 support
std::wstring wstringUnpack(const std::string& bytes)
{
    // TEMP
    return wstring_cast(bytes);
}
