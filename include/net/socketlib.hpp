#ifndef NET_SOCKET_LIB_HPP
#define NET_SOCKET_LIB_HPP

#include <string>
#include <sys/socket.h>

int sendBuf(int sock, const char* buf, int length);
int recvBuf(int sock, char* buf, int length);
std::string recvString(int sock);
int sendString(int sock, const std::string& s);
int sendFixed(int sock, const std::string& s);

void sendByte(int sock, uint8_t byte);
uint8_t recvByte(int sock);

uint8_t recvU8(int sock);
uint16_t recvU16(int sock);
uint32_t recvU32(int sock);
uint64_t recvU64(int sock);

int8_t recvS8(int sock);
int16_t recvS16(int sock);
int32_t recvS32(int sock);
int64_t recvS64(int sock);

void sendU8(int sock, uint8_t v);
void sendU16(int sock, uint16_t v);
void sendU32(int sock, uint32_t v);
void sendU64(int sock, uint64_t v);

void sendS8(int sock, int8_t v);
void sendS16(int sock, int16_t v);
void sendS32(int sock, int32_t v);
void sendS64(int sock, int64_t v);

void sendFloat32(int sock, float v);
void sendFloat64(int sock, double v);

float recvFloat32(int sock);
double recvFloat64(int sock);

#endif /* end of include guard: NET_SOCKET_LIB_HPP */
