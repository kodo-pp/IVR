#include <string>

#include <modbox/util/util.hpp>

std::wstring wstring_cast(const std::string& s)
{
    std::wstring ws;
    ws.reserve(s.length());
    for (char c : s) {
        ws += static_cast<wchar_t>(c);
    }
    return ws;
}
