#include <net/net.hpp>
#include <core/memory_manager.hpp>
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

// They are client threads, but we can call them server threads
// Or maybe vice versa
// I don't know, really
static std::unordered_set <std::thread*> serverThreads;

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
    log(L"Killing thread " << moduleListenerThread->native_handle());
    //pthread_kill(moduleListenerThread->native_handle(), SIGTERM);
    //moduleListenerThread->join();
    if (pthread_kill(moduleListenerThread->native_handle(), 0) != ESRCH) {
        pthread_cancel(moduleListenerThread->native_handle());
    }
    moduleListenerThread->join();
    memoryManager.freePtr(moduleListenerThread);

    serverThreadMutex.lock();
    for (const auto& thr : serverThreads) {
        log(L"Killing thread " << thr->native_handle());
        if (pthread_kill(thr->native_handle(), 0) != ESRCH) {
            //pthread_kill(thr->native_handle(), SIGTERM);
            pthread_cancel(thr->native_handle());
            //thr->join();
        }
        thr->join();
        memoryManager.freePtr(thr);
    }
    serverThreads.clear();
    serverThreadMutex.unlock();
    log(L"All them are dead");
}

void createModuleServerThread(int clientSocket) {
    serverThreadMutex.lock();
    std::thread* thr = new std::thread(moduleServerThreadFunc, clientSocket);
    if (!thr) {
        log(L"Unable to create thread");
        close(clientSocket);
        return;
    }
    log(L"Registering thread");
    serverThreads.insert(thr);
    log(L"Done");
    serverThreadMutex.unlock();
}


static void moduleListenerThreadFunc() {
    log(moduleListenerThread->native_handle());
    log(pthread_self());
    //kill(getpid(), SIGKILL);
    const uint16_t port = 44145; // int('MODBOX', 36) % 65536
    log(L"Creating a socket");
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        throw std::runtime_error("unable to create listening socket");
    }
    log(L"Socket created");

    union {
        struct sockaddr_in sockAddrIn;
        struct sockaddr sockAddr;
    } socketAddress;

    socketAddress.sockAddrIn.sin_family = AF_INET;
    socketAddress.sockAddrIn.sin_port = htons(port);
    socketAddress.sockAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);

    log(L"Trying to bind to 0.0.0.0:" << port);
    if (bind(listenSocket, &socketAddress.sockAddr, sizeof(socketAddress.sockAddrIn)) < 0) {
        throw std::runtime_error("unable to bind the socket");
    }

    listen(listenSocket, 1);
    log(L"Listening on 0.0.0.0:" << port);

    while (true) {
        int clientSocket = accept(listenSocket, nullptr, nullptr);
        log(L"Client connected");
        if (clientSocket < 0) {
            log(L"Unable to accept the connection from the client: accept() returned " << clientSocket);
            continue;
        }
        log(L"Spawning client thread");
        createModuleServerThread(clientSocket);
    }
}

static void moduleServerThreadFunc(int clientSocket) {
    log(L"Hi, I am thread with handle " << pthread_self());
    log(L"Let's say something to client!");
    std::string helloMessage = "Hello, our guest. You have 5 seconds to do nothing\n";
    send(clientSocket, helloMessage.c_str(), helloMessage.length(), 0);
    log(L"Hello, our guest!");
    sleep(5);
    std::string goodbyeMessage = "Time is up. Goodbye\n";
    send(clientSocket, goodbyeMessage.c_str(), goodbyeMessage.length(), 0);
    log(L"Bye-bye!");
    close(clientSocket);
    log(L"He's gone now...");
}
