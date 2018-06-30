#include <net/net.hpp>
#include <core/memory_manager.hpp>
#include <modules/module_manager.hpp>
#include <thread>
#include <atomic>
#include <stdexcept>
#include <mutex>
#include <unordered_set>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <log/log.hpp>
#include <iostream>
#include <util/util.hpp>
#include <misc/die.hpp>

// They are client threads, but we can call them server threads
// Or maybe vice versa
// I don't know, really
static std::unordered_set <pthread_t> serverThreads;

static std::thread* moduleListenerThread;
static std::mutex serverThreadMutex;

static void moduleListenerThreadFunc();
static void moduleServerThreadFunc(int);

void createModuleListenerThread() {
    moduleListenerThread = new std::thread(moduleListenerThreadFunc);
    if (moduleListenerThread == nullptr) {
        throw std::runtime_error("unable to create module listener thread");
    }
    memoryManager.track(moduleListenerThread);
}

void joinModuleListenerThread() {
    LOG(L"Killing thread " << moduleListenerThread->native_handle());
    //pthread_kill(moduleListenerThread->native_handle(), SIGTERM);
    //moduleListenerThread->join();
    if (pthread_kill(moduleListenerThread->native_handle(), 0) != ESRCH) {
        pthread_cancel(moduleListenerThread->native_handle());
    }
    moduleListenerThread->join();

    memoryManager.forget(moduleListenerThread);
    delete moduleListenerThread;

    serverThreadMutex.lock();
    for (const auto& thr : serverThreads) {
        LOG(L"Killing thread " << thr);
        if (pthread_kill(thr, 0) != ESRCH) {
            //pthread_kill(thr->native_handle(), SIGTERM);
            pthread_cancel(thr);
            //thr->join();
        }
        //thr->join(); // (It has been detached)
        //memoryManager.freePtr(thr);
    }
    serverThreads.clear();
    serverThreadMutex.unlock();
    LOG(L"All them are dead");
}

void createModuleServerThread(int clientSocket) {
    serverThreadMutex.lock();
    std::thread* thr = new std::thread(moduleServerThreadFunc, clientSocket);
    if (!thr) {
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


static void moduleListenerThreadFunc() {
//    LOG(moduleListenerThread->native_handle());
//    LOG(pthread_self());
    //kill(getpid(), SIGKILL);

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
                LOG(L"Unable to accept the connection from the client: accept() returned " << clientSocket);
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

static void moduleServerThreadFunc(int clientSocket) {
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
