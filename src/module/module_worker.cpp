#include <modules/module_manager.hpp>
#include <log/log.hpp>
#include <net/socketlib.hpp>
#include <exception>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <util/util.hpp>
#include <unistd.h>

static bool readModuleHeader(int sock) {
    char buf[8];
    recvBuf(sock, buf, 8);
    return memcmp(buf, "ModBox/m", 8ull) == 0;
}

static std::wstring readModuleName(int sock) {
    return wstring_cast(recvString(sock));
}

ModuleWorker::ModuleWorker(int _sock): sock(_sock) { }
ModuleWorker::~ModuleWorker() { }

void ModuleWorker::please_work() noexcept {
    try {
        work();
    } catch (std::exception e) {
        log(L"Module error: ModuleWorker::work() threw exception: '" << wstring_cast(e.what()) << L"'");
        return;
    } catch (...) {
        log(L"Module error: ModuleWorker::work() threw something we don't care about");
    }
}

void ModuleWorker::work() {
    std::string header("ModBox/M");
    sendString(sock, header);
    readModuleHeader(sock);
    std::wstring name = readModuleName(sock);
    log(L"Waiting for number");
    uint64_t num = recvU64(sock);
    log(L"Got number: " << num);
    log(L"Module '" << name << L"' connected");
    sleep(3);
    sendString(sock, std::string("Hello module '") + bytes_pack(name) + "'!\n");
    log(L"Exiting module worker");
}
