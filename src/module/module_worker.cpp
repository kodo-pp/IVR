#include <modules/module_manager.hpp>
#include <log/log.hpp>
#include <net/socketlib.hpp>
#include <core/core.hpp>
#include <exception>
#include <string>
#include <cstring>
#include <vector>
#include <sys/socket.h>
#include <util/util.hpp>
#include <unistd.h>
#include <modules/module_io.hpp>

ModuleWorker::ModuleWorker(int _sock): sock(_sock) { }
ModuleWorker::~ModuleWorker() { }

void ModuleWorker::please_work() noexcept {
    try {
        work();
    } catch (std::exception& e) {
        LOG(L"Module error: ModuleWorker::work() threw exception: '" << wstring_cast(e.what()) << L"'");
        return;
    } catch (...) {
        LOG(L"Module error: ModuleWorker::work() threw something we don't care about");
        return;
    }
}

void ModuleWorker::work() {
    std::string header("ModBox/M");
    sendFixed(sock, header);
    readModuleHeader(sock);
    std::wstring name = readModuleName(sock);
    LOG(L"Module '" << name << L"' connected");

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
            continue;
        } else if (handle == RESERVED_FP_HANDLE) {
            std::string internal_cmd = recvString(sock);
            if (internal_cmd == "exit") {
                sendString(sock, "exited");
                return;
            } else {
                throw std::runtime_error(std::string("invalid internal command: ") + internal_cmd);
            }
        }

        // Prepare to run it
        FuncProvider* prov;
        try {
            prov = getFuncProvider(handle);
        } catch (...) {
            throw std::runtime_error(std::string("Invalied handle: '") + std::to_string(handle));
        }
        if (prov == nullptr) {
            throw std::logic_error("getFuncProvider() returned nullptr");
        }
        
        ArgsSpec argsSpec = getArgsSpec(handle);

        // Read its arguments
        std::vector <void*> args;
        args.reserve(argsSpec.length());
        for (char i : argsSpec) {
            args.push_back(recvArg(sock, i));
        }

        // Run it
        struct FuncResult* result = prov->operator()(args);
        if (result == nullptr) {
            throw std::runtime_error("function provider error: result is nullptr");
        }

        // Send result back
        ArgsSpec retSpec = getRetSpec(handle);
        for (size_t i = 0; i < retSpec.length(); ++i) {
            sendArg(sock, result->data.at(i), retSpec.at(i));
        }

        // Free memory allocated for arguments...
        for (size_t i = 0; i < argsSpec.length(); ++i) {
            freeArg(args.at(i), argsSpec.at(i));
        }
        // ... and for returned data
        for (size_t i = 0; i < retSpec.length(); ++i) {
            freeArg(result->data.at(i), retSpec.at(i));
        }
        delete result;
    }

    LOG(L"Exiting module worker");
}
