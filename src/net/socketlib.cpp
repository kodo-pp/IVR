#include <cstring>
#include <exception>
#include <string>
#include <unordered_map>

#include <modbox/log/log.hpp>
#include <modbox/net/socketlib.hpp>
#include <modbox/util/util.hpp>

#include <sys/socket.h>

std::recursive_mutex netMutex;
std::unordered_map<int, std::vector<uint8_t>> buffers;

void putBuffer(int sock, const std::vector<uint8_t>& data)
{
    std::lock_guard<decltype(netMutex)> lock(netMutex);
    if (buffers.count(sock) == 0) {
        buffers.insert({sock, data});
    } else {
        buffers.at(sock).insert(buffers.at(sock).end(), data.begin(), data.end());
    }
}

template <typename Iterator>
void putBuffer(int sock, const Iterator& begin, const Iterator& end)
{
    std::lock_guard<decltype(netMutex)> lock(netMutex);
    if (buffers.count(sock) == 0) {
        buffers.insert({sock, {}});
    }
    buffers.at(sock).insert(buffers.at(sock).end(), begin, end);
}

void flushBuffer(int sock)
{
    std::lock_guard<decltype(netMutex)> lock(netMutex);
    if (buffers.count(sock) == 0) {
        throw std::logic_error(std::string("No such socket: ") + std::to_string(sock));
    }
    sendBuf(sock, buffers.at(sock).data(), buffers.at(sock).size());
    buffers.at(sock).clear();
}

void sendBuf(int sock, const void* buf, size_t length)
{
    size_t sent = 0;
    while (sent < length) {
        int remain = length - sent;
        const void* data = static_cast<const uint8_t*>(buf) + sent;
        int sent_now = send(sock, data, remain, 0);

        if (sent_now == -1) {
            // log("sendBuf: error sending data");
            throw std::runtime_error("sendBuf: error sending data");
        }
        sent += sent_now;
    }
}

void recvBuf(int sock, void* buf, size_t length)
{
    size_t received = 0;
    while (received < length) {
        int remain = length - received;
        void* data = static_cast<uint8_t*>(buf) + received;
        int received_now = recv(sock, data, remain, 0);

        if (received_now == -1) {
            // log("recvBuf: error receiving data");
            throw std::runtime_error("recvBuf: error receiving data");
        } else if (received_now == 0) {
            throw std::runtime_error("recvBuf: EOF reached");
        }
        received += received_now;
    }
}

std::string recvString(int sock)
{
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
    LOG("recvString: " << sock << ", '" << s << "'");
    return s;
}

void sendString(int sock, const std::string& s)
{
    LOG("sendString: " << sock << ", '" << s << "'");
    putBuffer(sock, s.begin(), s.end());
    putBuffer(sock, {0});
}
void sendFixed(int sock, const std::string& s)
{
    putBuffer(sock, s.begin(), s.end());
}

inline void sendByte(int sock, uint8_t byte)
{
    putBuffer(sock, {byte});
}

uint8_t recvByte(int sock)
{
    char byte;
    recvBuf(sock, &byte, 1);
    return static_cast<uint8_t>(byte);
}
