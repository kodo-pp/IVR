#include <cstdint>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <string>

#include <core/dyntype.hpp>
#include <log/log.hpp>
#include <util/util.hpp>

void* dyntypeNew(char type)
{
    switch (type) {
    case 'b': // signed byte
        return static_cast<void*>(new int8_t(0));
        break;
    case 'h': // signed short
        return static_cast<void*>(new int16_t(0));
        break;
    case 'i': // signed int
        return static_cast<void*>(new int32_t(0));
        break;
    case 'l': // signed longlong
        return static_cast<void*>(new int64_t(0));
        break;
    case 'B': // unsigned byte
        return static_cast<void*>(new uint8_t(0));
        break;
    case 'H': // unsigned short
        return static_cast<void*>(new uint16_t(0));
        break;
    case 'I': // unsigned int
        return static_cast<void*>(new uint32_t(0));
        break;
    case 'L': // unsigned longlong
        return static_cast<void*>(new uint64_t(0));
        break;
    case 'f': // float32
        return static_cast<void*>(new float(0));
        break;
    case 'F': // float64
        return static_cast<void*>(new double(0));
        break;
    case 's': // string
        return static_cast<void*>(new std::string(""));
        break;
    case 'w': // wstring
        return static_cast<void*>(new std::wstring(L""));
        break;
    case 'o': // blob
        return static_cast<void*>(new std::string(""));
        break;
    default: throw std::logic_error(std::string("dyntypeNew: unknown type: ") + type);
    }
}

void dyntypeDelete(void* val, char type)
{
    if (val == nullptr) {
        return;
    }
    switch (type) {
    case 'b': // signed byte
        delete static_cast<int8_t*>(val);
        break;
    case 'h': // signed short
        delete static_cast<int16_t*>(val);
        break;
    case 'i': // signed int
        delete static_cast<int32_t*>(val);
        break;
    case 'l': // signed longlong
        delete static_cast<int64_t*>(val);
        break;
    case 'B': // unsigned byte
        delete static_cast<uint8_t*>(val);
        break;
    case 'H': // unsigned short
        delete static_cast<uint16_t*>(val);
        break;
    case 'I': // unsigned int
        delete static_cast<uint32_t*>(val);
        break;
    case 'L': // unsigned longlong
        delete static_cast<uint64_t*>(val);
        break;
    case 'f': // float32
        delete static_cast<float*>(val);
        break;
    case 'F': // float64
        delete static_cast<double*>(val);
        break;
    case 's': // string
        delete static_cast<std::string*>(val);
        break;
    case 'w': // wstring
        delete static_cast<std::wstring*>(val);
        break;
    case 'o': delete static_cast<std::string*>(val); break;
    default: throw std::logic_error(std::string("dyntypeDelete: unknown type: ") + type);
    }
}
