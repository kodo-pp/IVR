#include <net/socketlib.hpp>
#include <log/log.hpp>
#include <util/util.hpp>
#include <sys/socket.h>
#include <string>
#include <cstring>
#include <exception>

int sendBuf(int sock, const char *buf, int length) {
    int sent = 0;
    while (sent < length) {
        int remain = length - sent;
        decltype(buf) data = buf + sent;
        int sent_now = send(sock, data, remain, 0);

        if (sent_now == -1) {
            //log("sendBuf: error sending data");
            throw std::runtime_error("sendBuf: error sending data");
        }
        sent += sent_now;
    }
    return sent;
}

int recvBuf(int sock, char *buf, int length) {
    int received = 0;
    while (received < length) {
        int remain = length - received;
        decltype(buf) data = buf + received;
        int received_now = recv(sock, data, remain, 0);

        if (received_now == -1) {
            //log("recvBuf: error receiving data");
            throw std::runtime_error("recvBuf: error receiving data");
        } else if (received_now == 0) {
            throw std::runtime_error("recvBuf: EOF reached");
        }
        received += received_now;
    }
    return received;
}

std::string recvString(int sock) {
    std::string s;
    char c;
    // TODO: maybe add buffers
    while (true) {
        recvBuf(sock, &c, 1);
        if (c == 0) {
            break;
        }
        s += c;
    }
    return s;
}

int sendString(int sock, const std::string& s) {
    auto sent = sendBuf(sock, s.c_str(), s.length() * sizeof(char));
    sendByte(sock, (uint8_t)0);
    return sent + 1;
}
int sendFixed(int sock, const std::string& s) {
    return sendBuf(sock, s.c_str(), s.length() * sizeof(char));
}

void sendByte(int sock, uint8_t byte) {
    char tmp = static_cast <char> (byte);
    sendBuf(sock, &tmp, 1);
}

uint8_t recvByte(int sock) {
    char byte;
    recvBuf(sock, &byte, 1);
    return static_cast <uint8_t> (byte);
}

uint8_t recvU8(int sock) {
    uint8_t c = recvByte(sock);
    return c;
}
uint16_t recvU16(int sock) {
    uint16_t v = 0;
    for (int i = 0; i < 2; ++i) {
        uint8_t c = recvByte(sock);
        v <<= 8;
        v |= c;
    }
    return intFlipEndian(v);
}
uint32_t recvU32(int sock) {
    uint32_t v = 0;
    for (int i = 0; i < 4; ++i) {
        uint8_t c = recvByte(sock);
        v <<= 8;
        v |= c;
    }
    return intFlipEndian(v);
}
uint64_t recvU64(int sock) {
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i) {
        uint8_t c = recvByte(sock);
        v <<= 8;
        v |= c;
    }
    return intFlipEndian(v);
}

int8_t recvS8(int sock) {
    return static_cast <int8_t> (recvU8(sock));
}
int16_t recvS16(int sock) {
    return static_cast <int16_t> (recvU16(sock));
}
int32_t recvS32(int sock) {
    return static_cast <int32_t> (recvU32(sock));
}
int64_t recvS64(int sock) {
    return static_cast <int64_t> (recvU64(sock));
}

void sendU8(int sock, uint8_t v) {
    sendByte(sock, v);
}
void sendU16(int sock, uint16_t v) {
    for (int i = 0; i < 2; ++i) {
        sendByte(sock, v & 0xFF);
        v >>= 8;
    }
}
void sendU32(int sock, uint32_t v) {
    for (int i = 0; i < 4; ++i) {
        sendByte(sock, v & 0xFF);
        v >>= 8;
    }
}
void sendU64(int sock, uint64_t v) {
    for (int i = 0; i < 8; ++i) {
        sendByte(sock, v & 0xFF);
        v >>= 8;
    }
}

void sendS8(int sock, int8_t v) {
    sendU8(sock, static_cast <uint8_t> (v));
}
void sendS16(int sock, int16_t v) {
    sendU16(sock, static_cast <uint16_t> (v));
}
void sendS32(int sock, int32_t v) {
    sendU32(sock, static_cast <uint32_t> (v));
}
void sendS64(int sock, int64_t v) {
    sendU64(sock, static_cast <uint64_t> (v));
}