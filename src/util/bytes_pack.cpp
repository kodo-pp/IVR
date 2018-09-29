#include <string>

#include <modbox/util/util.hpp>

std::string bytes_pack(const std::string& s)
{
    return s;
}

// TODO: add utf-8 support
std::string bytes_pack(const std::wstring& ws)
{
    std::string s;
    s.reserve(ws.length());
    for (wchar_t wc : ws) {
        if ((unsigned int)wc <= 0xFFu) {
            s += (char)wc;
        } else {
            s += '?';
        }
    }
    return s;
}
