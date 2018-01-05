#ifndef LOG_LOG_HPP
#define LOG_LOG_HPP

#include <vector>
#include <string>
#include <functional>
#include <cassert>

enum LogStreamSpecial {
    lssNewline,
    lssFlush,
    lssBeginLine
};

/**
 * Класс, представляющий поток для ведения логов
 *
 * Позволяет управлять списком потоков, на которые будет осуществляться вывод
 * логов.
 */
class LogStream {
protected:
    std::vector <std::ostream *> streamsVec;
    std::function <std::string()> lineBeginnerFunc;
    bool doBeginLine;
public:
    LogStream(const std::function <std::string()> &);
    LogStream(std::ostream *, const std::function <std::string()> &);
    LogStream(const std::vector <std::ostream *> &, const std::function <std::string()> &);

    virtual ~LogStream();

    std::vector <std::ostream *> & getStreamsVec();

    std::function <std::string()> getLineBeginnerFunc();
    void setLineBeginnerFunc(const std::function <std::string()> &);

    bool getDoBeginLine();

    void beginLine(bool);
    void flush();
};

LogStream & operator << (LogStream &, LogStreamSpecial);


template <class T>
LogStream & operator << (LogStream & logStream, T data) {
    auto streamsVec = logStream.getStreamsVec();
    for (auto & streamPtr : streamsVec) {
        assert(streamPtr != nullptr);
        if (logStream.getDoBeginLine()) {
            (*streamPtr) << logStream.getLineBeginnerFunc()();
        }
        (*streamPtr) << data;
        logStream.beginLine(false);
    }
    return logStream;
}

LogStream & getLogStream();

#endif /* end of include guard: LOG_LOG_HPP */
