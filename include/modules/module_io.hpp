#ifndef MODULES_MODULE_IO_HPP
#define MODULES_MODULE_IO_HPP

#include <string>

bool readModuleHeader(int sock);
std::wstring readModuleName(int sock);

void* recvArg(int sock, char spec);
void sendArg(int sock, void* arg, char spec);
void freeArg(void* arg, char spec);

#endif /* end of include guard: MODULES_MODULE_IO_HPP */
