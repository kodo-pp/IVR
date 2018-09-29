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
}
ModuleWorker::~ModuleWorker()
{
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

// TODO: implement it in module
void sendError(int sock, const std::string& errorMessage, uint8_t exitCode = 1)
{
    sendU8(sock, exitCode);
    sendString(sock, errorMessage);
    flushBuffer(sock);
}

void ModuleWorker::work()
{
    int sock = module.getMainSocket();
    LOG(L"Module '" << module.getName() << L"' connected");

    while (true) {
        // Receive command from module
        uint64_t handle = recvU64(sock);

        if (handle == 0) /* Null handle - intended to find handle for a command */ {
            std::string cmd = recvString(sock);
            try {
                uint64_t responseHandle = getFuncProviderHandle(cmd);
                sendU64(sock, responseHandle);
            } catch (const std::out_of_range& e) {
                sendU64(sock, 0);
            }
            flushBuffer(sock);
            continue;
        } else if (handle == RESERVED_FP_HANDLE) {
            std::string internal_cmd = recvString(sock);
            if (internal_cmd == "exit") {
                LOG("exiting");
                sendString(sock, "exited");
                flushBuffer(sock);
                return;
            } else {
                sendError(sock, std::string("invalid internal command: ") + internal_cmd);
            }
        }

        // Prepare to run it
        FuncProvider prov;
        prov = getFuncProvider(handle);

        ArgsSpec argsSpec = getArgsSpec(handle);

        // Read its arguments
        std::vector<void*> args;
        args.reserve(argsSpec.length());
        for (char i : argsSpec) {
            args.push_back(recvArg(sock, i));
        }

        // Run it
        FuncResult result;
        try {
            result = prov(args);
        } catch (const std::exception& e) {
            LOG("ModuleWorker: exception caught: " << e.what());
            sendError(sock, e.what());
            continue;
        }

        sendU8(sock, 0);

        // Send result back
        ArgsSpec retSpec = getRetSpec(handle);
        for (size_t i = 0; i < retSpec.length(); ++i) {
            sendArg(sock, result.data.at(i), retSpec.at(i));
        }
        flushBuffer(sock);

        // Free memory allocated for arguments...
        for (size_t i = 0; i < argsSpec.length(); ++i) {
            freeArg(args.at(i), argsSpec.at(i));
        }
        // ... and for returned data
        for (size_t i = 0; i < retSpec.length(); ++i) {
            freeArg(result.data.at(i), retSpec.at(i));
        }
    }

    LOG(L"Exiting module worker");
}

std::vector<void*> ModuleWorker::runModuleFunc(uint64_t id,
                                               const std::string& argTypes,
                                               const std::string& retTypes,
                                               const std::vector<void*> arguments)
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

        sendU64(sock, id);
        for (size_t i = 0; i < argTypes.length(); ++i) {
            sendArg(sock, arguments.at(i), argTypes.at(i));
        }
        flushBuffer(sock);
        for (size_t i = 0; i < argTypes.length(); ++i) {
            freeArg(arguments.at(i), argTypes.at(i));
        }
        uint8_t exitCode = recvU8(sock);
        if (exitCode != 0) {
            throw std::runtime_error(std::string("Module function exit code is ")
                                     + std::to_string(exitCode));
        }

        std::vector<void*> result;
        result.reserve(retTypes.length());
        for (char i : retTypes) {
            result.push_back(recvArg(sock, i));
        }

        return result;
    } catch (const std::exception& e) {
        LOG("Exception happened at ModuleWorker::runModuleFunc(): " << wstring_cast(e.what()));
        logStackTrace();
        throw e;
    }
}

std::pair<uint64_t, std::function<FuncResult(const std::vector<void*>&)>>
ModuleWorker::registerModuleFuncProvider(const std::string& name,
                                         std::string argTypes,
                                         std::string retTypes)
{
    uint64_t handle = moduleFuncs.insert(name);
    return {handle, [handle, argTypes, retTypes, this](const std::vector<void*> args) {
                FuncResult result;
                result.data = runModuleFunc(handle, argTypes, retTypes, args);
                return result;
            }};
}
