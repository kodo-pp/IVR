#ifndef MISC_DIE_HPP
#define MISC_DIE_HPP

#include <string>

#define die(text) do { \
    std::cerr << "Fatal error (" << __FILE__ << ":" << __LINE__ << "): " << (text) << std::endl; \
    exit(1); \
} while (0)

#define errdie(text, errmsg) do { \
    std::cerr << "Fatal error (" << __FILE__ << ":" << __LINE__ << "): " << (text) << ": " << errmsg << std::endl; \
    exit(1); \
} while (0)

#endif /* end of include guard: MISC_DIE_HPP */
