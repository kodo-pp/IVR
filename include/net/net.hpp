#ifndef NET_NET_HPP
#define NET_NET_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

void createModuleListenerThread();
void joinModuleListenerThread();
void createModuleServerThread();

#endif /* end of include guard: NET_NET_HPP */
