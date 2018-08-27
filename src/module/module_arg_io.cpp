#include <cstring>
#include <stdexcept>
#include <string>

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
        throw std::runtime_error("Invalid module header");
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
        uint64_t size = recvU64(sock);
        std::string* bytes = new std::string;
        bytes->reserve(size);
        for (size_t i = 0; i < size; ++i) {
            bytes->push_back(static_cast<char>(recvByte(sock)));
        }
        arg = (void*)bytes;
    } break;
    default: throw std::logic_error(std::string("recvArg: unknown type: ") + spec);
    }
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
        sendU64(sock, ((std::string*)arg)->length());
        sendBuf(sock, ((std::string*)arg)->c_str(), (int)((std::string*)arg)->length());
    } break;
    default: throw std::logic_error(std::string("sendArg: unknown type: ") + spec);
    }
}

void freeArg(void* arg, char spec)
{
    switch (spec) {
    case 'b': // signed byte
        delete (int8_t*)arg;
        break;
    case 'h': // signed short
        delete (int16_t*)arg;
        break;
    case 'i': // signed int
        delete (int32_t*)arg;
        break;
    case 'l': // signed longlong
        delete (int64_t*)arg;
        break;
    case 'B': // unsigned byte
        delete (uint8_t*)arg;
        break;
    case 'H': // unsigned short
        delete (uint16_t*)arg;
        break;
    case 'I': // unsigned int
        delete (uint32_t*)arg;
        break;
    case 'L': // unsigned longlong
        delete (uint64_t*)arg;
        break;
    case 'f': // float32
        delete (float*)arg;
        break;
    case 'F': // float64
        delete (double*)arg;
        break;
    case 's': // string
        delete (std::string*)arg;
        break;
    case 'w': // wstring
        delete (std::wstring*)arg;
        break;
    case 'o': // blob
        delete (std::string*)arg;
        break;
    default: throw std::logic_error(std::string("freeArg: unknown type: ") + spec);
    }
}
