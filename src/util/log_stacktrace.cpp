#include <modbox/log/log.hpp>
#include <modbox/util/util.hpp>

#ifdef NO_BOOST_STACKTRACE
void logStackTrace()
{
    LOG("Stack trace:");
    LOG("  <unavailable as boost::stacktrace was disabled at build-time>");
}

#else

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

#endif // NO_BOOST_STACKTRACE
