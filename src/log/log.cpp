#include <atomic>
#include <chrono>
#include <codecvt>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <locale>
#include <mutex>

#include <log/log.hpp>

static std::recursive_mutex lbfMutex;
std::recursive_mutex logMutex;

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

    struct tm* tm_now = new struct tm;
    localtime_r(&c_now, tm_now);
    char* lineBegin;

    // Current time
    // Example:
    // [LOG: 23.11.2039 16:44:37]
    asprintf(&lineBegin,
             "[LOG: %02d.%02d.%d %02d:%02d:%02d] T~%04u | ",
             tm_now->tm_mday,
             tm_now->tm_mon + 1,
             tm_now->tm_year + 1900,
             tm_now->tm_hour,
             tm_now->tm_min,
             tm_now->tm_sec,
             (uint)pthread_self() % 10000);

    std::wstring stlLineBegin(lineBegin, lineBegin + strlen(lineBegin));
    free(lineBegin);

    lbfMutex.lock();
    cachedLineBegin = stlLineBegin;
    lbfMutex.unlock();

    delete tm_now;
    return stlLineBegin;
});

static LogStream& logStreamLink = logStream;

LogStream& getLogStream()
{
    return logStreamLink;
}
