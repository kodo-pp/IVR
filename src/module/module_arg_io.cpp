#include <cstring>
#include <stdexcept>
#include <string>

#include <log/log.hpp>
#include <modules/module_io.hpp>
#include <net/socketlib.hpp>
#include <util/util.hpp>

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

std::wstring readModuleName(int sock)
{
    return wstring_cast(recvString(sock));
}

void* recvArg(int sock, char spec)
{
    void* arg = nullptr;
    switch (spec) {
    case 'b': // signed byte
        arg = (void*)(new int8_t(recvS8(sock)));
        break;
    case 'h': // signed short
        arg = (void*)(new int16_t(recvS16(sock)));
        break;
    case 'i': // signed int
        arg = (void*)(new int32_t(recvS32(sock)));
        break;
    case 'l': // signed longlong
        arg = (void*)(new int64_t(recvS64(sock)));
        break;
    case 'B': // unsigned byte
        arg = (void*)(new uint8_t(recvU8(sock)));
        break;
    case 'H': // unsigned short
        arg = (void*)(new uint16_t(recvU16(sock)));
        break;
    case 'I': // unsigned int
        arg = (void*)(new uint32_t(recvU32(sock)));
        break;
    case 'L': // unsigned longlong
        arg = (void*)(new uint64_t(recvU64(sock)));
        break;
    case 'f': // float32
        arg = (void*)(new float(recvFloat32(sock)));
        break;
    case 'F': // float64
        arg = (void*)(new double(recvFloat64(sock)));
        break;
    case 's': // string
        arg = (void*)(new std::string(recvString(sock)));
        break;
    case 'w': // wstring
        arg = (void*)(new std::wstring(wstringUnpack(recvString(sock))));
        break;
    case 'o': { // blob
        arg = (void*)(new std::vector<uint8_t>(recvBlob(sock)));
    } break;
    default:
        throw std::logic_error(std::string("recvArg: unknown type: ") + spec);
    }
    memoryManager.track(arg);
    return arg;
}

void sendArg(int sock, void* arg, char spec)
{
    switch (spec) {
    case 'b': // signed byte
        sendS8(sock, *(int8_t*)arg);
        break;
    case 'h': // signed short
        sendS16(sock, *(int16_t*)arg);
        break;
    case 'i': // signed int
        sendS32(sock, *(int32_t*)arg);
        break;
    case 'l': // signed longlong
        sendS64(sock, *(int64_t*)arg);
        break;
    case 'B': // unsigned byte
        sendU8(sock, *(uint8_t*)arg);
        break;
    case 'H': // unsigned short
        sendU16(sock, *(uint16_t*)arg);
        break;
    case 'I': // unsigned int
        sendU32(sock, *(uint32_t*)arg);
        break;
    case 'L': // unsigned longlong
        sendU64(sock, *(uint64_t*)arg);
        break;
    case 'f': // float32
        sendFloat32(sock, *(float*)arg);
        break;
    case 'F': // float64
        sendFloat64(sock, *(double*)arg);
        break;
    case 's': // string
        sendString(sock, *(std::string*)arg);
        break;
    case 'w': // wstring
        sendString(sock, bytes_pack(*(std::wstring*)arg));
        break;
    case 'o': { // blob
        sendBlob(sock, *static_cast<std::vector<uint8_t>*>(arg));
    } break;
    default:
        throw std::logic_error(std::string("sendArg: unknown type: ") + spec);
    }
}

void freeArg(void* arg, char spec)
{
    switch (spec) {
    case 'b': // signed byte
        memoryManager.deletePtr<int8_t>(arg);
        break;
    case 'h': // signed short
        memoryManager.deletePtr<int16_t>(arg);
        break;
    case 'i': // signed int
        memoryManager.deletePtr<int32_t>(arg);
        break;
    case 'l': // signed longlong
        memoryManager.deletePtr<int64_t>(arg);
        break;
    case 'B': // unsigned byte
        memoryManager.deletePtr<uint8_t>(arg);
        break;
    case 'H': // unsigned short
        memoryManager.deletePtr<uint16_t>(arg);
        break;
    case 'I': // unsigned int
        memoryManager.deletePtr<uint32_t>(arg);
        break;
    case 'L': // unsigned longlong
        memoryManager.deletePtr<uint64_t>(arg);
        break;
    case 'f': // float32
        memoryManager.deletePtr<float>(arg);
        break;
    case 'F': // float64
        memoryManager.deletePtr<double>(arg);
        break;
    case 's': // string
        memoryManager.deletePtr<std::string>(arg);
        break;
    case 'w': // wstring
        memoryManager.deletePtr<std::wstring>(arg);
        break;
    case 'o': // blob
        memoryManager.deletePtr<std::vector<uint8_t>>(arg);
        break;
    default:
        throw std::logic_error(std::string("freeArg: unknown type: ") + spec);
    }
}
