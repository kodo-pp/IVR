#include <atomic>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <unordered_set>

#include <core/memory_manager.hpp>
#include <log/log.hpp>
#include <misc/die.hpp>
#include <modules/module_manager.hpp>
#include <net/net.hpp>
#include <util/util.hpp>

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
static void moduleServerThreadFunc(int /*clientSocket*/);

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

    // Join is a better solution, but it is hharder to implement as we need
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

void createModuleServerThread(int clientSocket)
{
    serverThreadMutex.lock();
    std::thread* thr = new std::thread(moduleServerThreadFunc, clientSocket);
    if (thr == nullptr) {
        LOG(L"Unable to create thread");
        close(clientSocket);
        return;
    }
    LOG(L"Registering thread");
    serverThreads.insert(thr->native_handle());
    LOG(L"Detaching thread");
    thr->detach();
    LOG(L"Deleting thread object");
    delete thr;
    LOG(L"Done");
    serverThreadMutex.unlock();
}

static void moduleListenerThreadFunc()
{
    //    LOG(moduleListenerThread->native_handle());
    //    LOG(pthread_self());
    // kill(getpid(), SIGKILL);

    try {
        const uint16_t port = 44145; // int('MODBOX', 36) % 65536
        LOG(L"Creating a socket");
        int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (listenSocket < 0) {
            throw std::runtime_error("unable to create listening socket");
        }
        LOG(L"Socket created");

        union {
            struct sockaddr_in sockAddrIn;
            struct sockaddr sockAddr;
        } socketAddress;

        socketAddress.sockAddrIn.sin_family = AF_INET;
        socketAddress.sockAddrIn.sin_port = htons(port);
        socketAddress.sockAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);

        LOG(L"Trying to bind to 0.0.0.0:" << port);
        if (bind(listenSocket, &socketAddress.sockAddr, sizeof(socketAddress.sockAddrIn)) < 0) {
            throw std::runtime_error("unable to bind the socket");
        }

        listen(listenSocket, 1);
        LOG(L"Listening on 0.0.0.0:" << port);

        while (true) {
            int clientSocket = accept(listenSocket, nullptr, nullptr);
            LOG(L"Client connected");
            if (clientSocket < 0) {
                LOG(L"Unable to accept the connection from the client: accept() returned "
                    << clientSocket);
                continue;
            }
            LOG(L"Spawning client thread");
            createModuleServerThread(clientSocket);
        }
    } catch (std::exception& e) {
        LOG(L"FATAL error (at " __FILE__ "): " << wstring_cast(e.what()));
        LOG(L"Exiting");
        exit(1);
    }
}

static void moduleServerThreadFunc(int clientSocket)
{
    ModuleWorker worker(clientSocket);
    /*
    try {
        worker.work();
    } catch (std::exception& e) {
        LOG(L"module error: ModuleWorker threw exception: '" << wstring_cast(e.what()) << L"'");
    } catch (...) {
        LOG(L"module error: ModuleWorker threw something which we don't care about");
    }
    */
    worker.please_work();
    close(clientSocket);
}
