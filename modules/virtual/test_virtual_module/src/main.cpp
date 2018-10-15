#include <string>
#include <vector>

#include <modbox/log/log.hpp>
#include <modbox/util/util.hpp>

#include <unistd.h>

extern "C" void module_main(UNUSED const std::vector<std::string>& args)
{
    LOG("Hello from virtual module!");
    sleep(10);
    LOG("Good bye from virtual module!");
}
