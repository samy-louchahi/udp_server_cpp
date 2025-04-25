#pragma once

#include <vector>
#include <netinet/in.h>
#include "map/spatial_point.hpp"

namespace map_server {

class UdpReceiver {
public:
    explicit UdpReceiver(int port);
    ~UdpReceiver();

    bool start();
    std::vector<SpatialPoint> receive_packet();

private:
    int port_;
    int sockfd_;
    struct sockaddr_in servaddr_;
};

}

