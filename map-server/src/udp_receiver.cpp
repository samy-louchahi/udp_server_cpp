#include "map/udp_receiver.hpp"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <vector>
#include <array>

// Define ColorRGBA structure
struct ColorRGBA {
    uint8_t r, g, b, a;
};

namespace map_server {

UdpReceiver::UdpReceiver(int port)
    : port_(port), sockfd_(-1) {
    // Création du socket UDP
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_ < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse
    memset(&servaddr_, 0, sizeof(servaddr_));
    servaddr_.sin_family = AF_INET;
    servaddr_.sin_addr.s_addr = INADDR_ANY;
    servaddr_.sin_port = htons(port_);

    // Bind le socket à l'adresse
    if (bind(sockfd_, (const struct sockaddr*)&servaddr_, sizeof(servaddr_)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "[UdpReceiver] Listening on UDP port " << port_ << std::endl;
}

UdpReceiver::~UdpReceiver() {
    if (sockfd_ >= 0) {
        close(sockfd_);
    }
}

bool UdpReceiver::start() {
    return true;
}

std::vector<SpatialPoint> UdpReceiver::receive_packet() {
    constexpr size_t BUF_SIZE = 1500;
    std::array<uint8_t, BUF_SIZE> buffer{};

    socklen_t len = sizeof(servaddr_);
    ssize_t n = recvfrom(sockfd_, buffer.data(), buffer.size(), MSG_WAITALL,
                         (struct sockaddr*)&servaddr_, &len);

    if (n < 0) {
        perror("recvfrom failed");
        return {};
    }

    std::vector<SpatialPoint> points;
    const size_t POINT_SIZE = 12; // 4 + 1 + 1 + 1 + 1 + 4 = 12 bytes

    if (n % POINT_SIZE != 0) {
        std::cerr << "[UdpReceiver] Received malformed packet of size " << n << std::endl;
        return {};
    }

    for (size_t i = 0; i < static_cast<size_t>(n); i += POINT_SIZE) {
        uint32_t morton_code;
        std::memcpy(&morton_code, &buffer[i], sizeof(uint32_t));

        int32_t x, y, z;
        SpatialPoint encode_point;
        encode_point.decode_coordinate(morton_code, x, y, z);

        uint8_t r = buffer[i + 4];
        uint8_t g = buffer[i + 5];
        uint8_t b = buffer[i + 6];
        uint8_t a = buffer[i + 7];

        float timestamp;
        std::memcpy(&timestamp, &buffer[i + 8], sizeof(float));

        ColorRGBA color = { r, g, b, a };
        SpatialPoint point(x, y, z, color, timestamp);
        points.push_back(point);
    }

    return points;
}

} // namespace map_server



