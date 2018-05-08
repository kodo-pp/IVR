#ifndef NET_NET_HPP
#define NET_NET_HPP

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <thread>

void createModuleListenerThread();
void joinModuleListenerThread();
void createModuleServerThread();

#endif /* end of include guard: NET_NET_HPP */
