#include <fstream>
#include <iostream>
#include <mutex>

#include <log/log.hpp>

LogStream::LogStream(const std::function<std::wstring()>& _lineBeginnerFunc) :
        lineBeginnerFunc(_lineBeginnerFunc),
        doBeginLine(true)
{
    // std::cout << "Constr 1" << std::endl;
    streamsVec.push_back(&std::wcerr);

    const char logFileName[]
            = "modbox.log"; // COMBAK: Заменить на значение, заданное аргументами командной строки
    std::wostream* logFile = new std::wofstream(logFileName,
                                                std::ios::out); // COMBAK: добавить MemoryManager
    // assert(logFile->is_open()); // COMBAK: Бросать std::runtime_error или делать что-то подобное
    streamsVec.push_back(logFile);
    allocatedVec.push_back(logFile);
}

LogStream::LogStream(std::wostream* stream,
                     const std::function<std::wstring()>& _lineBeginnerFunc) :
        lineBeginnerFunc(_lineBeginnerFunc),
        doBeginLine(true)
{
    // std::cout << "Constr 2" << std::endl;
    assert(stream != nullptr);
    streamsVec.push_back(stream);
}

LogStream::LogStream(const std::vector<std::wostream*>& _streamsVec,
                     const std::function<std::wstring()>& _lineBeginnerFunc) :
        lineBeginnerFunc(_lineBeginnerFunc),
        doBeginLine(true),
        streamsVec(_streamsVec)
{
    // std::cout << "Constr 3" << std::endl;
}

LogStream::~LogStream()
{
    for (auto& i : allocatedVec) {
        delete i;
    }
}

std::vector<std::wostream*>& LogStream::getStreamsVec()
{
    std::vector<std::wostream*>& streamsVecLink = streamsVec;
    return streamsVecLink;
}

std::function<std::wstring()> LogStream::getLineBeginnerFunc()
{
    return lineBeginnerFunc;
}

void LogStream::setLineBeginnerFunc(const std::function<std::wstring()>& _lineBeginnerFunc)
{
    lineBeginnerFunc = _lineBeginnerFunc;
}

void LogStream::flush()
{
    for (auto& streamPtr : streamsVec) {
        assert(streamPtr != nullptr);
        streamPtr->flush();
    }
}

void LogStream::beginLine(bool v = true)
{
    doBeginLine = v;
}

bool LogStream::getDoBeginLine()
{
    return doBeginLine;
}

LogStream& operator<<(LogStream& logStream, LogStreamSpecial data)
{
    switch (data) {
    case lssNewline: logStream << "\n" << lssBeginLine << lssFlush; break;
    case lssBeginLine: logStream.beginLine(); break;
    case lssFlush: logStream.flush();
    }
    return logStream;
}
