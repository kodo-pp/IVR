#ifndef MISC_TYPE_ASSERTIONS_HPP
#define MISC_TYPE_ASSERTIONS_HPP

#include <cstdint>
#include <type_traits>

#define TYPESIZE_ASSERT(type, size)                                                                \
    static_assert(sizeof(type) == size, "sizeof(" #type ") != " #size)

TYPESIZE_ASSERT(uint8_t, 1);
TYPESIZE_ASSERT(uint16_t, 2);
TYPESIZE_ASSERT(uint32_t, 4);
TYPESIZE_ASSERT(uint64_t, 8);
TYPESIZE_ASSERT(int8_t, 1);
TYPESIZE_ASSERT(int16_t, 2);
TYPESIZE_ASSERT(int32_t, 4);
TYPESIZE_ASSERT(int64_t, 8);
TYPESIZE_ASSERT(float, 4);
TYPESIZE_ASSERT(double, 8);

#undef TYPESIZE_ASSERT

#endif /* end of include guard: MISC_TYPE_ASSERTIONS_HPP */
