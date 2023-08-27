#include "UdpClient.hpp"
#include <Arduino.h>
#include <cerrno>
#include <lwip/netdb.h>
#include <lwip/sockets.h>

LightUDP::LightUDP()
        : udp_server(-1), server_port(0), remote_port(0), remote_ip_int(0) {}

uint8_t LightUDP::begin(IPAddress address, uint16_t port) {
    server_port = port;

    if ((udp_server = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        Serial.printf("could not create socket: %d", errno);
        return 0;
    }

    int yes = 1;
    if (setsockopt(udp_server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) <
        0) {
        Serial.printf("could not set socket option: %d\n", errno);

        return 0;
    }

    struct sockaddr_in addr{};
    memset((char *) &addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);
    addr.sin_addr.s_addr = (in_addr_t) address;
    if (bind(udp_server, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        Serial.printf("could not bind socket: %d\n", errno);
        return 0;
    }
    fcntl(udp_server, F_SETFL, O_NONBLOCK);
    return 1;
}

void LightUDP::setServer(const char *host, uint16_t port) {
    struct hostent *server;
    server = gethostbyname(host);
    if (server == nullptr) {
        Serial.printf("could not get host from dns: %d\n", errno);
        return;
    }
    setServer(IPAddress((const uint8_t *) (server->h_addr_list[0])), port);
}

void LightUDP::setServer(IPAddress address, uint16_t port) {
    this->remote_ip = address;
    this->remote_ip_int = (uint32_t) remote_ip;
    this->remote_port = port;
}


void LightUDP::send(uint8_t *buf, size_t len) {
    if (udp_server == -1) {
        if ((udp_server = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            Serial.printf("could not create socket: %d\n", errno);
            return;
        }
        fcntl(udp_server, F_SETFL, O_NONBLOCK);
    }

    struct sockaddr_in recipient{};
    recipient.sin_addr.s_addr = remote_ip_int;
    recipient.sin_family = AF_INET;
    recipient.sin_port = htons(remote_port);
    int sent = sendto(udp_server, buf, len, 0, (struct sockaddr *) &recipient,
                      sizeof(recipient));
    if (sent < 0) {
        Serial.printf("could not send data: %d\n", errno);
    }
}

int LightUDP::recv(uint8_t *buf, size_t size) {
    struct sockaddr_in si_other{};
    int slen = sizeof(si_other), len;
    if ((len = recvfrom(udp_server, buf, size, MSG_DONTWAIT, (struct sockaddr *) &si_other, (socklen_t *) &slen)) ==
        -1) {
        if (errno == EWOULDBLOCK) {
            return 0;
        }
        log_e("could not receive data: %d", errno);

    }
    this->rec_remote_ip = IPAddress(si_other.sin_addr.s_addr);
    this->rec_remote_port = ntohs(si_other.sin_port);
    return len;

}

uint8_t LightUDP::beginMulticast(const IPAddress& a, uint16_t p) {
    if (begin(IPAddress(INADDR_ANY), p)) {
        if (a != 0) {
            struct ip_mreq mreq{};
            mreq.imr_multiaddr.s_addr = (in_addr_t) a;
            mreq.imr_interface.s_addr = INADDR_ANY;
            if (setsockopt(udp_server, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
                log_e("could not join igmp: %d", errno);
                return 0;
            }
            multicast_ip = a;
        }
        return 1;
    }
    return 0;
}

const IPAddress &LightUDP::getRecRemoteIp() const {
    return rec_remote_ip;
}

uint16_t LightUDP::getRecRemotePort() const {
    return rec_remote_port;
}

const IPAddress &LightUDP::getMulticastIp() const {
    return multicast_ip;
}

void LightUDP::endMulticast() {
    if(multicast_ip != 0){
        struct ip_mreq mreq{};
        mreq.imr_multiaddr.s_addr = (in_addr_t)multicast_ip;
        mreq.imr_interface.s_addr = (in_addr_t)0;
        setsockopt(udp_server, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
        multicast_ip = IPAddress(INADDR_ANY);
    }
}
