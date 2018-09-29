#include <cstdint>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <string>

#include <modbox/core/dyntype.hpp>
#include <modbox/core/memory_manager.hpp>
#include <modbox/log/log.hpp>
#include <modbox/util/util.hpp>

void* _dyntypeNew(char type)
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
        return static_cast<void*>(new std::vector<uint8_t>());
        break;
    default:
        throw std::logic_error(std::string("dyntypeNew: unknown type: ") + type);
    }
}

void dyntypeDelete(void* val, char type)
{
    if (val == nullptr) {
        return;
    }
    switch (type) {
    case 'b': // signed byte
        memoryManager.deletePtr<int8_t>(val);
        break;
    case 'h': // signed short
        memoryManager.deletePtr<int16_t>(val);
        break;
    case 'i': // signed int
        memoryManager.deletePtr<int32_t>(val);
        break;
    case 'l': // signed longlong
        memoryManager.deletePtr<int64_t>(val);
        break;
    case 'B': // unsigned byte
        memoryManager.deletePtr<uint8_t>(val);
        break;
    case 'H': // unsigned short
        memoryManager.deletePtr<uint16_t>(val);
        break;
    case 'I': // unsigned int
        memoryManager.deletePtr<uint32_t>(val);
        break;
    case 'L': // unsigned longlong
        memoryManager.deletePtr<uint64_t>(val);
        break;
    case 'f': // float32
        memoryManager.deletePtr<float>(val);
        break;
    case 'F': // float64
        memoryManager.deletePtr<double>(val);
        break;
    case 's': // string
        memoryManager.deletePtr<std::string>(val);
        break;
    case 'w': // wstring
        memoryManager.deletePtr<std::wstring>(val);
        break;
    case 'o': // blob
        memoryManager.deletePtr<std::vector<uint8_t>>(val);
        break;
    default:
        throw std::logic_error(std::string("dyntypeDelete: unknown type: ") + type);
    }
}

void* dyntypeNew(char type)
{
    auto ret = _dyntypeNew(type);
    memoryManager.track(ret);
    return ret;
}
