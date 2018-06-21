#ifndef UTIL_TESTIT_HPP
#define UTIL_TESTIT_HPP

#include <iostream>
#include <log/log.hpp>
#include <util/util.hpp>
#include <cstdlib>

#ifdef DEBUG_MODE
    static int _testit_failed = 0;
    static std::string _testit_name = "";

    static void testit_func(bool expr, const char* exprStr, const char* filename, int line) {
        if (expr) {
            LOG(L"passed    : " << wstring_cast(exprStr) << "\t| " << wstring_cast(filename) << L":" << line);
        } else {
            LOG(L"FAILED!!! : " << wstring_cast(exprStr) << "\t| " << wstring_cast(filename) << L":" << line);
            ++_testit_failed;
        }
    }

    #define testit(expr) \
        testit_func(expr, #expr, __FILE__, __LINE__)

    #define testit_begin(name) \
        do { \
            LOG(L"Beginning a test of " << wstring_cast(name)); \
            _testit_name = name; \
        } while (false)
    
    #define testit_end() \
        do { \
            LOG(L"Test of " << wstring_cast(_testit_name) << " finished"); \
            if (_testit_failed > 0) { \
                LOG(_testit_failed << L" test(s) failed. Aborting"); \
                abort(); \
            } \
        } while (false)
                

#else // if not DEBUG_MODE
    #define testit(expr)
    #define testit_begin(expr)
    #define testit_end(expr)
#endif // DEBUG_MODE

#endif // end of include guard: UTIL_TESTIT_HPP


