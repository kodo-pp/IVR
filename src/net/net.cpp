#include <atomic>
#include <cstring>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <unordered_set>

#include <modbox/core/memory_manager.hpp>
#include <modbox/log/log.hpp>
#include <modbox/misc/die.hpp>
#include <modbox/modules/module.hpp>
#include <modbox/modules/module_io.hpp>
#include <modbox/modules/module_manager.hpp>
#include <modbox/net/net.hpp>
#include <modbox/net/socketlib.hpp>
#include <modbox/util/util.hpp>

#include <netinet/in.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// They are client threads, but we can call them server threads
// Or maybe vice versa
// I don't know, really
static std::unordered_set<pthread_t> serverThreads;

static std::thread* moduleListenerThread;
static std::mutex serverThreadMutex;

static void moduleListenerThreadFunc();
static void moduleServerThreadFunc(Module&& module);

void createModuleListenerThread()
{
    moduleListenerThread = new std::thread(moduleListenerThreadFunc);
    if (moduleListenerThread == nullptr) {
        throw std::runtime_error("unable to create module listener thread");
    }
    memoryManager.track(moduleListenerThread);
}

void joinModuleListenerThread()
{
    // Actually we just don't need to kill those threads, they are killed at exit
    // (at least on my Linux). After a 'return' from main() there is a exit_group
    // syscall (see man 2 exit_group), which terminates all process group (including
    // detached threads). However I have just created a big problem for myself when
    // I would port ModBox on Windows... At least I hope MinGW/Cygwin has done most
    // of this work already.

    // Join is a better solution, but it is harder to implement as we need
    // to signal the thread to terminate. Now we just detach it and then it
    // is killed by exit_group(2).
    moduleListenerThread->detach();

    memoryManager.forget(moduleListenerThread);
    delete moduleListenerThread;

    serverThreadMutex.lock();
    serverThreads.clear();
    serverThreadMutex.unlock();

    LOG(L"All them are dead");
}

void createModuleServerThread(Module&& module)
{
    std::lock_guard<std::mutex> lock(serverThreadMutex);
    std::thread* thr = new std::thread(moduleServerThreadFunc, module);
    if (thr == nullptr) {
        LOG(L"Unable to create thread");
        module.cleanup();
        return;
    }
    LOG(L"Registering thread");
    serverThreads.insert(thr->native_handle());
    LOG(L"Detaching thread");
    thr->detach();
    LOG(L"Deleting thread object");
    delete thr;
    LOG(L"Done");
}

int createListeningSocket(uint64_t port)
{
    LOG(L"Creating a socket");
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Set SO_REUSEADDR and SO_REUSEPORT socket option.
    // Thanks to https://stackoverflow.com/questions/24194961/how-do-i-use-setsockoptso-reuseaddr
    int enable = 1;
    LOG("Setting SO_REUSEADDR...");
    if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        LOG("WARNING: setsockopt(SO_REUSEADDR) failed");
    }
    enable = 1;
    LOG("Setting SO_REUSEPORT...");
    if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int))) {
        LOG("WARNING: setsockopt(SO_REUSEPORT) failed");
    }
    if (listenSocket < 0) {
        throw std::runtime_error("unable to create listening socket");
    }
    LOG(L"Socket created");

    union
    {
        struct sockaddr_in sockAddrIn;
        struct sockaddr sockAddr;
    } socketAddress;

    socketAddress.sockAddrIn.sin_family = AF_INET;
    socketAddress.sockAddrIn.sin_port = htons(port);
    socketAddress.sockAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);

    LOG(L"Trying to bind to 0.0.0.0:" << port);
    if (bind(listenSocket, &socketAddress.sockAddr, sizeof(socketAddress.sockAddrIn)) < 0) {
        // throw std::runtime_error("unable to bind the socket");
        throw std::runtime_error(strerror(errno));
    }

    listen(listenSocket, 1);
    LOG(L"Listening on 0.0.0.0:" << port);
    return listenSocket;
}

int acceptSocket(int listenSocket)
{
    int clientSocket = accept(listenSocket, nullptr, nullptr);
    LOG(L"Client connected");
    if (clientSocket < 0) {
        LOG(L"Unable to accept the connection from the client: accept() returned " << clientSocket);
        throw std::runtime_error("accept() failed");
    }
    return clientSocket;
}

static void moduleListenerThreadFunc()
{
    //    LOG(moduleListenerThread->native_handle());
    //    LOG(pthread_self());
    // kill(getpid(), SIGKILL);

    try {
        const uint16_t mainPort = 44145;    // int('MODBOX', 36) % 65536
        const uint16_t reversePort = 54144; // reversed("44145")
        int mainListeningSocket = createListeningSocket(mainPort);
        int reverseListeningSocket = createListeningSocket(reversePort);

        // Модули, которые уже подключились к первому сокету, но ещё не подключились ко второму
        // Ключ - имя модуля, значение - сокет
        std::unordered_map<std::string, int> pendingModules;

        while (true) {
            // TODO: переписать с асинхронным IO или потоками

            // Ждём, пока кто-то постучится в основной порт
            int mainClientSocket = acceptSocket(mainListeningSocket);
            // Читаем служебную информацию, в том числе имя модуля
            sendFixed(mainClientSocket, std::string("ModBox/M"));
            flushBuffer(mainClientSocket);
            readModuleHeader(mainClientSocket);
            std::string moduleName = recvString(mainClientSocket);

            // Запоминаем, что он подключился к основному порту
            pendingModules.insert({moduleName, mainClientSocket});

            // Ждём, пока кто-то постучится в обратный порт
            int reverseClientSocket = acceptSocket(reverseListeningSocket);
            // Читаем служебную информацию, в том числе имя модуля
            sendFixed(reverseClientSocket, std::string("ModBox/R"));
            flushBuffer(reverseClientSocket);
            readReverseModuleHeader(reverseClientSocket);
            std::string reverseModuleName = recvString(reverseClientSocket);
            // Смотрим, подключился ли он к основному порту
            if (pendingModules.count(reverseModuleName) == 0) {
                // Если нет, то что-то тут не так. Надо придумать защиту от дурака,
                // Но с синхронным IO в один поток это сложно. Поэтому просто вываливаемся

                // TODO: написать RAII-обёртку над сокетаими
                // Либо использовать готовую библиотеку
                close(mainClientSocket);
                close(reverseClientSocket);
                close(mainListeningSocket);
                close(reverseListeningSocket);
                throw std::runtime_error("Module connected to reverse port but not to main");
            }

            // Удаляем его из списка pending
            int mainModuleSocket = pendingModules.at(reverseModuleName);
            int reverseModuleSocket = reverseClientSocket;
            pendingModules.erase(reverseModuleName);

            // Получаем список зависимостей

            // И запускаем moduleWorker
            LOG(L"Spawning client thread");

            // TODO: dependencies
            createModuleServerThread(
                    Module(mainModuleSocket, reverseModuleSocket, reverseModuleName, {}));

            // COMBAK: old code
            // dependencies.reserve(dependenciesCount);
            // for (uint64_t i = 0; i < dependenciesCount; ++i) {
            //     dependencies.emplace_back(recvString(mainModuleSocket));
            // }
            // createModuleServerThread(
            //         Module(mainModuleSocket, reverseModuleSocket, reverseModuleName,
            //         dependencies));
        }
    } catch (std::exception& e) {
        LOG(L"FATAL error (at " __FILE__ "): " << wstring_cast(e.what()));
        destroy();
    }
}

static void moduleServerThreadFunc(Module&& module)
{
    ModuleWorker worker(std::move(module));
    /*
    try {
        worker.work();
    } catch (std::exception& e) {
        LOG(L"module error: ModuleWorker threw exception: '" << wstring_cast(e.what()) << L"'");
    } catch (...) {
        LOG(L"module error: ModuleWorker threw something which we don't care about");
    }
    */
    moduleManager.registerModuleWorker(worker);
    worker.please_work();
    module.cleanup();
}
