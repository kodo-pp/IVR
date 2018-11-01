#include <cstring>
#include <exception>
#include <string>
#include <vector>

#include <modbox/core/core.hpp>
#include <modbox/log/log.hpp>
#include <modbox/modules/module_io.hpp>
#include <modbox/modules/module_manager.hpp>
#include <modbox/net/socketlib.hpp>
#include <modbox/util/util.hpp>

#include <sys/socket.h>
#include <unistd.h>

ModuleWorker::ModuleWorker(Module&& _module) : module(_module)
{
    moduleManager.registerModule(module);
}
ModuleWorker::~ModuleWorker()
{
    moduleManager.unregisterModule(module.getName());
}

void ModuleWorker::please_work() noexcept
{
    try {
        work();
    } catch (std::exception& e) {
        LOG(L"Module error: ModuleWorker::work() threw exception: '" << wstring_cast(e.what())
                                                                     << L"'");
        return;
    } catch (...) {
        LOG(L"Module error: ModuleWorker::work() threw something we don't care about");
        return;
    }
}

Module& ModuleWorker::getModule()
{
    return module;
}

void sendError(int sock, const std::string& errorMessage, uint8_t exitCode = 1)
{
    sendString(sock, DyntypeCaster<std::string>::get(exitCode));
    sendString(sock, errorMessage);
    flushBuffer(sock);
}

void ModuleWorker::work()
{
    int sock = module.getMainSocket();
    LOG(L"Module '" << module.getName() << L"' connected");

    while (true) {
        std::string command = recvString(sock);

        // Prepare to run it
        FuncProvider prov = getFuncProvider(command);
        ArgsSpec argsSpec = getArgsSpec(command);

        // Read its arguments
        std::vector<std::string> args;
        args.reserve(argsSpec.length());
        for (UNUSED char i : argsSpec) {
            args.push_back(recvString(sock));
        }

        // Run it
        FuncResult result;
        try {
            result = prov(args);
        } catch (const std::exception& e) {
            LOG("ModuleWorker: exception caught: " << e.what());
            sendString(sock, "1");
            sendString(sock, e.what());
            continue;
        }

        sendString(sock, "0");

        // Send result back
        ArgsSpec retSpec = getRetSpec(command);
        for (size_t i = 0; i < retSpec.length(); ++i) {
            sendString(sock, result.data.at(i));
        }
        flushBuffer(sock);
    }

    LOG(L"Exiting module worker");
}

std::vector<std::string> ModuleWorker::runModuleFunc(const std::string& command,
                                                     const std::string& argTypes,
                                                     const std::string& retTypes,
                                                     const std::vector<std::string> arguments)
{
    try {
        if (argTypes.size() != arguments.size()) {
            LOG("Error at ModuleWorker::runModuleFunc(): arguments.size() != argTypes.size()  @ "
                "argTypes = "
                << wstring_cast(argTypes) << ", arguments.size == " << arguments.size());
            throw std::logic_error("arguments.size() != argTypes.size()");
        }
        std::lock_guard<std::mutex> lock(reverseMutex);
        int sock = module.getReverseSocket();

        sendString(sock, command);
        for (size_t i = 0; i < argTypes.length(); ++i) {
            sendString(sock, arguments.at(i));
        }
        flushBuffer(sock);

        int exitCode = DyntypeCaster<int>::get(recvString(sock));
        if (exitCode != 0) {
            throw std::runtime_error(std::string("Module function exit code is ")
                                     + std::to_string(exitCode));
        }

        std::vector<std::string> result;
        result.reserve(retTypes.length());
        for (UNUSED char i : retTypes) {
            result.push_back(recvString(sock));
        }
        return result;
    } catch (const std::exception& e) {
        LOG("Exception happened at ModuleWorker::runModuleFunc(): " << wstring_cast(e.what()));
        close(module.getMainSocket());
        close(module.getReverseSocket());
        logStackTrace();
        std::rethrow_exception(std::current_exception());
    }
}

std::function<FuncResult(const std::vector<std::string>&)> ModuleWorker::registerModuleFuncProvider(
        const std::string& name,
        std::string argTypes,
        std::string retTypes)
{
    moduleFuncs.insert(name);
    return [name, argTypes, retTypes, this](const std::vector<std::string> args) {
        FuncResult result;
        result.data = runModuleFunc(name, argTypes, retTypes, args);
        return result;
    };
}
