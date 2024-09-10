#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <map>
#include <vector>

#define PROG_SUCCESS 0
#define PROG_FAILED 1
#define FUNC_ERROR -1

#define PORT 1234
#define MC_ADDR "224.0.0.5"
#define BUFF_SIZE 1
#define SEND_TIMEOUT 3
#define RECV_TIMEOUT 1
#define CONNECTION_TIMEOUT SEND_TIMEOUT * 2

struct sockaddr_in_comparator {
    bool operator()(const sockaddr_in& lhs, const sockaddr_in& rhs) const {
        if (lhs.sin_addr.s_addr < rhs.sin_addr.s_addr) return true;
        if (lhs.sin_addr.s_addr > rhs.sin_addr.s_addr) return false;
        return lhs.sin_port < rhs.sin_port;
    }
};

int expired(struct timespec *lastsend_time, int timeout) {
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    if (current_time.tv_sec - lastsend_time->tv_sec > timeout) {
        lastsend_time->tv_nsec = current_time.tv_nsec;
        lastsend_time->tv_sec = current_time.tv_sec;
        return 1;
    }
    return 0;
}

int make_recv_socket() {
    int sock_recv;
    int err;
    struct sockaddr_in sockaddr_recv;
    struct ip_mreq mreq;
    struct timeval recv_timeout;
    recv_timeout.tv_sec = RECV_TIMEOUT;
    recv_timeout.tv_usec = 0;

    sock_recv = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_recv == FUNC_ERROR) {
        printf("socket error\n");
        return FUNC_ERROR;
    }

    memset((char*)&sockaddr_recv, 0, sizeof(struct sockaddr_in));
	sockaddr_recv.sin_family = AF_INET;
	sockaddr_recv.sin_port = htons(PORT);// the unsigned short integer hostshort from host byte order to network byte order
    sockaddr_recv.sin_addr.s_addr = htonl(INADDR_ANY); //the unsigned integer hostlong from host byte order to network byte order
                                                         //INADDR_ANY 0.0.0.0 т.е. любой локальный адрес
                                                        
    int optval = 1;
    err = setsockopt(sock_recv, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (err == FUNC_ERROR) {
        printf("setsockopt error\n");
        close(sock_recv);
        return FUNC_ERROR;
    }

    err = bind(sock_recv, (struct sockaddr*) &sockaddr_recv, sizeof(sockaddr_recv));
    if (err == FUNC_ERROR) {
        printf("bind error\n");
        close(sock_recv);
        return FUNC_ERROR;
    }

    mreq.imr_multiaddr.s_addr = inet_addr(MC_ADDR);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY); 

    err = setsockopt(sock_recv, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if (err == FUNC_ERROR) {
        printf("setsockopt error\n");
        close(sock_recv);
        return FUNC_ERROR;
    }

    err = setsockopt(sock_recv, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));
    if (err == FUNC_ERROR) {
        printf("setsockopt error\n");
        close(sock_recv);
        return FUNC_ERROR;
    }

    return sock_recv;
}

int make_send_socket(struct sockaddr_in *sockaddr_send) {
    int err;
    int sock_send;

    sock_send = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_send == FUNC_ERROR) {
        printf("socket error\n");
        return FUNC_ERROR;
    }

    memset((char*)sockaddr_send, 0, sizeof(struct sockaddr_in));
	sockaddr_send->sin_family = AF_INET;
	sockaddr_send->sin_port = htons(PORT);// the unsigned short integer hostshort from host byte order to network byte order
	
    err = inet_aton(MC_ADDR, &sockaddr_send->sin_addr); // the Internet host address cp from the IPv4  numbers-and-dots  notation  into binary form (in network byte order) and stores it in the structure that inp points to
	if (err == 0) {
		printf("inet_aton error\n");
        close(sock_send);
        return PROG_FAILED;
	}

    return sock_send;
}

void print_map(const std::map<struct sockaddr_in, struct timespec, sockaddr_in_comparator> *copies) {
    for (const auto& [st_addr, timer] : *copies) {
        printf("Copy: %s:%d\n", inet_ntoa(st_addr.sin_addr), ntohs(st_addr.sin_port));
    }
    printf("\n");
}

int main(void) {
    int err;
    int sock_recv, sock_send;
    struct sockaddr_in sockaddr_send, sockaddr_copy;
    socklen_t sockaddr_copy_len = sizeof(sockaddr_copy);
    struct timespec lastsend_time, lastrecv_time;
    char buff[BUFF_SIZE];
    memset(buff, 0, BUFF_SIZE);
    std::map<struct sockaddr_in, struct timespec, sockaddr_in_comparator> copies;
    std::vector<struct sockaddr_in> disconnected_addrs;

    sock_recv = make_recv_socket();
    if (sock_recv == FUNC_ERROR) {
        return PROG_FAILED;
    }
    sock_send = make_send_socket(&sockaddr_send);
    if (sock_recv == FUNC_ERROR) {
        close(sock_recv);
        return PROG_FAILED;
    }

    clock_gettime(CLOCK_MONOTONIC, &lastsend_time);

    while (1) {
		err = recvfrom(sock_recv, buff, BUFF_SIZE, 0, (struct sockaddr*) &sockaddr_copy, &sockaddr_copy_len);
        if (err != FUNC_ERROR) {
            clock_gettime(CLOCK_MONOTONIC, &lastrecv_time);
            if (copies.count(sockaddr_copy) == 0) {
                printf("New copy: %s:%d\n", inet_ntoa(sockaddr_copy.sin_addr), ntohs(sockaddr_copy.sin_port));
                print_map(&copies);
            }
            copies[sockaddr_copy] = lastrecv_time;
		}

        if (expired(&lastsend_time, SEND_TIMEOUT)) {
            err = sendto(sock_send, buff, BUFF_SIZE, 0, (struct sockaddr*) &sockaddr_send, sizeof(sockaddr_send));
            if (err == FUNC_ERROR) {
                printf("sendto error\n");
                close(sock_recv);
                close(sock_send);
                return PROG_FAILED;
            }
        }

        for (auto& [st_addr, timer] : copies) {
            if (expired(&timer, CONNECTION_TIMEOUT)) {
                printf("Disconnected copy: %s:%d\n\n", inet_ntoa(st_addr.sin_addr), ntohs(st_addr.sin_port));
                disconnected_addrs.push_back(st_addr);
            }
        }
        int vector_size = disconnected_addrs.size();
        for (int i = 0; i < vector_size; i++) {
            copies.erase(disconnected_addrs.back());
            disconnected_addrs.pop_back();
        }
        if (vector_size > 0) {
            print_map(&copies);
        }
    }

    // if (setsockopt(sock_recv, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
    //     printf("setsockopt error\n");
    //     close(sock_recv);
    //     close(sock_send);
    //     return PROG_FAILED;
    // }
    //close(sock_recv);
    //close(sock_send);
}