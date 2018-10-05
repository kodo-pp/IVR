#ifndef NET_SOCKET_LIB_HPP
#define NET_SOCKET_LIB_HPP

#include <string>
#include <vector>

#include <sys/socket.h>

void sendBuf(int sock, const void* buf, size_t length);
void recvBuf(int sock, void* buf, size_t length);
std::string recvString(int sock);
void sendString(int sock, const std::string& s);
void sendFixed(int sock, const std::string& s);
void flushBuffer(int sock);

void sendByte(int sock, uint8_t byte);
uint8_t recvByte(int sock);

#endif /* end of include guard: NET_SOCKET_LIB_HPP */
