#include <modbox/modules/module.hpp>

#include <unistd.h>

Module::Module(int _mainSocket, int _reverseSocket, const std::string& _name)
        : mainSocket(_mainSocket), reverseSocket(_reverseSocket), name(_name)
{
}

Module::Module(const Module& other)
        : mainSocket(other.mainSocket), reverseSocket(other.reverseSocket), name(other.name)
{
}

void Module::cleanup() noexcept
{
}

Module::~Module()
{
    close(mainSocket);
    close(reverseSocket);
}

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
