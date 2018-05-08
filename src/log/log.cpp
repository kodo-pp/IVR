#include <log/log.hpp>
#include <mutex>
#include <atomic>
#include <chrono>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <locale>
#include <codecvt>

static std::mutex lbfMutex;
std::mutex logMutex;

static std::wstring cachedLineBegin;
static std::atomic<time_t> prevTime(0);

// OK, too lazy to cope with references, so let's pray and hope that compiler
// will optimize it to move semantics
static LogStream logStream([]() -> std::wstring {
    auto stl_now = std::chrono::system_clock::now();
    time_t c_now = std::chrono::system_clock::to_time_t(stl_now);

    if (c_now == prevTime) {
        lbfMutex.lock();
        std::wstring copied = cachedLineBegin;
        lbfMutex.unlock();
        return copied;
    }
    prevTime = c_now;

    struct tm * tm_now = new struct tm;
    localtime_r(&c_now, tm_now);
    char* lineBegin;

    // Current time
    // Example:
    // [LOG: 23.11.2039 16:44:37]
    asprintf(&lineBegin, "[LOG: %02d.%02d.%d %02d:%02d:%02d] ",
             tm_now->tm_mday,
             tm_now->tm_mon + 1,
             tm_now->tm_year + 1900,
             tm_now->tm_hour,
             tm_now->tm_min,
             tm_now->tm_sec);

    std::wstring stlLineBegin;

    // Now wee need to convert narrow string to wide one.
    // lineBegin is guaranted to contain only ASCII characters, so conversion is simple
    // WARNING: use of pointers is unsafe
    {
        char* tmp = lineBegin;
        while (*tmp != (char)0) {
            stlLineBegin.push_back(*tmp);
            ++tmp;
        }
    }

    free(lineBegin);

    lbfMutex.lock();
    cachedLineBegin = stlLineBegin;
    lbfMutex.unlock();

    delete tm_now;
    return stlLineBegin;
});

static LogStream & logStreamLink = logStream;

LogStream & getLogStream() {
    return logStreamLink;
}
