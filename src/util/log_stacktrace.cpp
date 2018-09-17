#include <log/log.hpp>
#include <util/util.hpp>

#include <boost/stacktrace/frame.hpp>
#include <boost/stacktrace/stacktrace.hpp>

void logStackTrace()
{
    LOG("Stack trace:");
    for (const auto& frame : boost::stacktrace::stacktrace().as_vector()) {
        LOG("  at " << wstring_cast(frame.name().empty() ? "??" : frame.name()) << " ("
                    << frame.address() << ")");
    }
}
