#ifndef LOG_LOG_HPP
#define LOG_LOG_HPP

#include <cassert>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

#include <core/destroy.hpp>

extern std::recursive_mutex logMutex;

enum LogStreamSpecial
{
    lssNewline,
    lssFlush,
    lssBeginLine
};

#define LOG(data)                                                                                  \
    do {                                                                                           \
        if (!areWeShuttingDown) {                                                                  \
            std::lock_guard<std::recursive_mutex> log_lock(logMutex);                              \
            getLogStream() << data << lssNewline;                                                  \
        }                                                                                          \
    } while (false)

/**
 * Класс, представляющий поток для ведения логов
 *
 * Позволяет управлять списком потоков, на которые будет осуществляться вывод
 * логов.
 */
class LogStream
{
protected:
    std::function<std::wstring()> lineBeginnerFunc;
    bool doBeginLine;
    std::vector<std::wostream*> streamsVec;
    std::vector<std::wostream*> allocatedVec;

public:
    explicit LogStream(const std::function<std::wstring()>&);
    explicit LogStream(std::wostream*, const std::function<std::wstring()>&);
    explicit LogStream(const std::vector<std::wostream*>&, const std::function<std::wstring()>&);

    virtual ~LogStream();

    std::vector<std::wostream*>& getStreamsVec();

    std::function<std::wstring()> getLineBeginnerFunc();
    void setLineBeginnerFunc(const std::function<std::wstring()>&);

    bool getDoBeginLine();

    void beginLine(bool);
    void flush();
};

LogStream& operator<<(LogStream&, LogStreamSpecial);

template <class T>
LogStream& operator<<(LogStream& logStream, T data)
{
    auto streamsVec = logStream.getStreamsVec();
    for (auto& streamPtr : streamsVec) {
        assert(streamPtr != nullptr);
        if (logStream.getDoBeginLine()) {
            (*streamPtr) << logStream.getLineBeginnerFunc()();
        }
    }
    for (auto& streamPtr : streamsVec) {
        (*streamPtr) << data;
        logStream.beginLine(false);
    }
    return logStream;
}

LogStream& getLogStream();

#endif /* end of include guard: LOG_LOG_HPP */
