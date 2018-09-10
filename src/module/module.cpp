#include <modules/module.hpp>

#include <unistd.h>

Module::Module(int _mainSocket,
               int _reverseSocket,
               const std::wstring& _name,
               ModuleId _id,
               const ModuleLanguage& _language)
        : mainSocket(_mainSocket)
        , reverseSocket(_reverseSocket)
        , id(_id)
        , name(_name)
        , executableFileName(L"<unset>")
        , language(_language)
{
}

Module::Module(const Module& other)
        : mainSocket(other.mainSocket)
        , reverseSocket(other.reverseSocket)
        , id(other.id)
        , name(other.name)
        , executableFileName(other.executableFileName)
        , language(other.language)
{
}
Module& Module::operator=(const Module& other)
{
    Module tmp(other);
    std::swap(*this, tmp);
    return *this;
}

void Module::cleanup() noexcept
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    close(mainSocket);
    close(reverseSocket);
}
ModuleId Module::getModuleId() const
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    return id;
}

std::wstring Module::getName() const
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    return name;
}
void Module::setName(const std::wstring& newName)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    name = newName;
}

std::wstring Module::getExecutableFileName() const
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    return executableFileName;
}
void Module::setExecutableFileName(const std::wstring& newExecFileName)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    executableFileName = newExecFileName;
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
