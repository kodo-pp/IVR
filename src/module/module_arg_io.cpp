#include <cstring>
#include <stdexcept>
#include <string>

#include <modbox/log/log.hpp>
#include <modbox/modules/module_io.hpp>
#include <modbox/net/socketlib.hpp>
#include <modbox/util/util.hpp>

void readModuleHeader(int sock)
{
    char buf[8];
    recvBuf(sock, buf, 8);
    if (memcmp(buf, "ModBox/m", 8ull) != 0) {
        throw std::runtime_error("Invalid module header");
    }
}

void readReverseModuleHeader(int sock)
{
    char buf[8];
    recvBuf(sock, buf, 8);
    if (memcmp(buf, "ModBox/r", 8ull) != 0) {
        throw std::runtime_error("Invalid reverse module header");
    }
}

std::string readModuleName(int sock)
{
    return recvString(sock);
}
