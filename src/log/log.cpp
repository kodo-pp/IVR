#include <log/log.hpp>

static LogStream logStream([]() -> std::string { return "[LOG] "; }); // COMBAK

static LogStream & logStreamLink = logStream;

LogStream & getLogStream() {
    return logStreamLink;
}
