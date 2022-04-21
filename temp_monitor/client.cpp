#include "../common/conf.h"
#include "client.h"

#ifdef _WIN32
    #include <Winsock2.h>
    #include <WS2tcpip.h>
    #include <Windows.h>
    #define strerror(errno) WSAGetLastError()
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <errno.h>
    #include <cstring>
#endif

#include <string>
#include <chrono>
#include <iostream>
#include <functional>

static int fd = 0;
static sockaddr_in addr { };

bool clientInit(std::string srcAddr, unsigned short port) {
    #ifdef _WIN32
    WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
    #endif

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cout << "Error creating socket: " << strerror(errno) << std::endl;
        return false;
    }

    unsigned int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)) < 0) {
        std::cout << "Error setting SO_REUSEADDR on socket: " << strerror(errno) << std::endl;
        return false;
    }

    // Source addr
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // differs from sender
    addr.sin_port = htons(port);

    if (bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cout << "Error binding socket: " << strerror(errno) << std::endl;
        return false;
    }

    // Group
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(srcAddr.c_str());
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    // Tell kernel to join group
    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0) {
        std::cout << "Error joining group: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

void clientLoop(std::function<void(SensorData, ControlData)> callback) {
    Packet buff;
    #ifdef _WIN32
    int addrlen = sizeof(addr);
    #else
    unsigned int addrlen = sizeof(addr);
    #endif

    while (true) {
        if (recvfrom(fd, (char*)&buff, sizeof(Packet), 0, (sockaddr*)&addr, &addrlen) < 0) {
            std::cout << "Error recieving packet: " << strerror(errno) << std::endl;
        }

        SensorData sd { };
        sd.timeNow = std::chrono::system_clock::from_time_t(buff.timeNow);
        sd.inletTemp = buff.inletTemp;
        sd.exhaustTemp = buff.exhaustTemp;
        sd.totalPower = buff.totalPower;
        
        for (int i = 0; i < CPU_N; i++)
            sd.cpuTemps[i] = buff.cpuTemps[i];

        for (int i = 0; i < FAN_N; i++)
            sd.fanSpeeds[i] = buff.fanSpeeds[i];

        ControlData cd;
        if (buff.control) {
            cd.control = true;
            cd.tempAvg = buff.tempAvg;
            cd.ctrlSpeed = buff.ctrlSpeed;
        }
        else cd.control = false;

        callback(sd, cd);
    }
}