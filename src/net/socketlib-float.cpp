#include <net/socketlib.hpp>
#include <cstring>
#include <type_traits>
#include <misc/type_assertions.hpp>
#include <util/util.hpp>

// Floats are guaranted to be 4-bytes long, and doubles are 8-bytes, thanks to
// misc/type_assertions.hpp

// Send/receive floating-point numbers in IEEE 754 format

UNUSED static void sendIeeeFloat32(int sock, float v) {
    uint32_t buf;

    memcpy(&buf, &v, sizeof(buf));
    sendU32(sock, buf);
}
UNUSED static void sendIeeeFloat64(int sock, double v) {
    uint64_t buf;

    memcpy(&buf, &v, sizeof(buf));
    sendU64(sock, buf);
}

UNUSED static float recvIeeeFloat32(int sock) {
    uint32_t buf;
    buf = recvU32(sock);

    float ret;
    memcpy(&ret, &buf, sizeof(buf));
    return ret;
}

UNUSED static double recvIeeeFloat64(int sock) {
    uint64_t buf;
    buf = recvU64(sock);

    double ret;
    memcpy(&ret, &buf, sizeof(buf));
    return ret;
}

// Send/receive floating-point numbers in string format (TEMP, may cause VERY big
// performance issues)

static void sendStringFloat32(int sock, float v) {
    std::string str = std::move(std::to_string(v));
    sendString(sock, std::move(str));
}

static void sendStringFloat64(int sock, double v) {
    std::string str = std::move(std::to_string(v));
    sendString(sock, std::move(str));
}

static float recvStringFloat32(int sock) {
    std::string str = std::move(recvString(sock));
    return std::stof(std::move(str));
}

static double recvStringFloat64(int sock) {
    std::string str = std::move(recvString(sock));
    return std::stod(std::move(str));
}

// These are functions that may choose one or another way to transmit floating-point
// number (depending on support for features like IEEE 754 by module)
// TODO: add optional IEEE 754 floating-point number support

void sendFloat32(int sock, float v) {
    sendStringFloat32(sock, v);
}

void sendFloat64(int sock, double v) {
    sendStringFloat64(sock, v);
}

float recvFloat32(int sock) {
    return recvStringFloat32(sock);
}

double recvFloat64(int sock) {
    return recvStringFloat64(sock);
}
