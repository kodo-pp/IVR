#ifndef MODULES_MODULE_HPP
#define MODULES_MODULE_HPP

#include <memory>
#include <mutex>
#include <string>
#include <vector>
/**
 * Represents a module
 */

class Module
{
public:
    Module(int _mainSocket,
           int _reverseSocket,
           const std::string& _name,
           const std::vector<std::string>& _dependencies);
    Module(const Module& other);
    Module(Module&& other) = default;
    virtual ~Module() = default;

    Module& operator=(const Module& other) = default;
    Module& operator=(Module&& other) = default;

    void cleanup() noexcept;

    std::string getName() const;

    int getMainSocket() const;
    int getReverseSocket() const;

    std::vector<std::string> getDependencies() const;

protected:
    int mainSocket;
    int reverseSocket;
    std::string name;
    std::vector<std::string> dependencies;
    mutable std::recursive_mutex mtx;
};

#endif /* end of include guard: MODULES_MODULE_HPP */
