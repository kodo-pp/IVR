#include <string>
#include <vector>

#include <modbox/log/log.hpp>
#include <modbox/util/util.hpp>

extern "C" void module_main(UNUSED const std::vector<std::string>& args)
{
    LOG("Hello from virtual module!");
}
