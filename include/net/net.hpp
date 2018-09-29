#ifndef NET_NET_HPP
#define NET_NET_HPP

#include <thread>

#include <modbox/modules/module.hpp>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void createModuleListenerThread();
void joinModuleListenerThread();
void createModuleServerThread(Module&& module);

#endif /* end of include guard: NET_NET_HPP */
