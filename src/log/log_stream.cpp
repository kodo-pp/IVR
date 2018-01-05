#include <log/log.hpp>
#include <iostream>
#include <fstream>

/*
class LogStream {
protected:
    std::vector <std::ostream *> streamsVec;
    std::function <std::string()> lineBeginnerFunc;
public:
    LogStream(const std::function <std::string()> &);
    LogStream(const std::ostream *, const std::function <std::string()> &);
    LogStream(const std::vector <std::ostream *> &, const std::function <std::string()> &);

    virtual ~LogStream();

    std::vector <std::ostream *> & getStreamsVec();

    std::function <std::string()> getLineBeginnerFunc();
    void setLineBeginnerFunc(const std::function <std::string()> &);

    void flush();
};
*/

LogStream::LogStream(const std::function <std::string()> & _lineBeginnerFunc) {
    //std::cout << "Constr 1" << std::endl;
    doBeginLine = true;
    lineBeginnerFunc = _lineBeginnerFunc;
    streamsVec.push_back(&std::cerr);

    const char logFileName[] = "IVR.log"; // COMBAK: Заменить на значение, заданное аргументами командной строки
    std::ostream * logFile = new std::ofstream(logFileName, std::ios::out); // COMBAK: добавить MemoryManager
    //assert(logFile->is_open()); // COMBAK: Бросать std::runtime_error или делать что-то подобное
    streamsVec.push_back(logFile);
}

LogStream::LogStream(std::ostream * stream, const std::function <std::string()> & _lineBeginnerFunc) {
    //std::cout << "Constr 2" << std::endl;
    doBeginLine = true;
    lineBeginnerFunc = _lineBeginnerFunc;
    assert (stream != nullptr);
    streamsVec.push_back(stream);
}

LogStream::LogStream(const std::vector <std::ostream *> & _streamsVec, const std::function <std::string()> & _lineBeginnerFunc) {
    //std::cout << "Constr 3" << std::endl;
    doBeginLine = true;
    lineBeginnerFunc = _lineBeginnerFunc;
    streamsVec = _streamsVec;
}

LogStream::~LogStream() {}

std::vector <std::ostream *> & LogStream::getStreamsVec() {
    std::vector <std::ostream *> & streamsVecLink = streamsVec;
    return streamsVecLink;
}

std::function <std::string()> LogStream::getLineBeginnerFunc() {
    return lineBeginnerFunc;
}

void LogStream::setLineBeginnerFunc(const std::function <std::string()> & _lineBeginnerFunc) {
    lineBeginnerFunc = _lineBeginnerFunc;
}

void LogStream::flush() {
    for (auto & streamPtr : streamsVec) {
        assert(streamPtr != nullptr);
        streamPtr->flush();
    }
}

void LogStream::beginLine(bool v = true) {
    doBeginLine = v;
}

bool LogStream::getDoBeginLine() {
    return doBeginLine;
}


LogStream & operator << (LogStream & logStream, LogStreamSpecial data) {
    switch (data) {
    case lssNewline:
        logStream << "\n" << lssBeginLine << lssFlush;
        break;
    case lssBeginLine:
        logStream.beginLine();
        break;
    case lssFlush:
        logStream.flush();
    }
    return logStream;
}
