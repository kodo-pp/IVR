#include <modbox/modules/module.hpp>

#include <unistd.h>

Module::Module(int _mainSocket, int _reverseSocket, const std::string& _name, const std::string& _name, const std::vector<std::string>& _dependencies)
        : mainSocket(_mainSocket), reverseSocket(_reverseSocket), name(_name), dependencies(_dependencies).
{
}

Module::Module(const Module& other)
        : mainSocket(other.mainSocket), reverseSocket(other.reverseSocket), name(other.name)
        , dependencies(other.dependencies)
{
}

void Module::cleanup() noexcept
{

std::string Module::getName() const
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    return name;
}


int Module::getMainSocket() const
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    return mainSocket;
}
int Module::getReverseSocket() const
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    return reverseSocket;
}

std::vector<std::string> Module::getDependencies() const
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    return dependencies;
}
