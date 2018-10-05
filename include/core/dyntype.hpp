#ifndef CORE_DYNTYPE_HPP
#define CORE_DYNTYPE_HPP

#include <modbox/util/base64.hpp>

#include <boost/lexical_cast.hpp>

template <typename To>
struct DyntypeCaster
{
    template <typename From>
    static To get(const From& value)
    {
        return boost::lexical_cast<To>(value);
    }

    static To get(const std::vector<uint8_t>& blob)
    {
        std::string s = base64_encode(blob);
        return boost::lexical_cast<To>(s);
    }
};

template <>
struct DyntypeCaster<std::vector<uint8_t>>
{
    template <typename From>
    static std::vector<uint8_t> get(const From& value)
    {
        auto s = boost::lexical_cast<std::string>(value);
        return base64_decode(s);
    }
};

#endif /* end of include guard: CORE_DYNTYPE_HPP */
