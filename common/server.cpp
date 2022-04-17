#include "conf.h"
#include "server.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstring>

#include <iostream>

static int fd = 0;
static sockaddr_in addr { };

bool serverInit(std::string dstAddr, unsigned short port) {
    // Create ordinary UDP socket
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cout << "Error creating socket: " << strerror(errno) << std::endl;
        return false;
    }

    // Destination multicast address
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(dstAddr.c_str());
    addr.sin_port = htons(port);

    return true;
}

bool serverSend(Packet packet) {
    if (sendto(fd, (char*)&packet, sizeof(Packet), 0, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cout << "Error sending packet: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}