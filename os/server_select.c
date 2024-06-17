#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h> //sockaddr_in
#include <unistd.h>
#include <arpa/inet.h> //inet_ntoa
#include <sys/time.h> //struct timeval
#include <sys/un.h> //fd_set
#include <poll.h> //struct pollfd

#define PROG_SUCCESS 0
#define PROG_FAILED 1
#define FUNC_ERROR -1

#define FUNC_SUCCESS 0
#define ACCEPT_ERROR 1
#define READ_ERROR 2
#define WRITE_ERROR 3
#define SELECT_ERROR 4

#define BUFF_SIZE 1024
#define MAX_CLIENT 5
#define PORT 8888
#define IP "127.0.0.1"

void prepare_select(int* max_fd, int* num, fd_set* rfds, int* srv_sock, int* clt_sock) {
    FD_ZERO(rfds);
    FD_SET(*srv_sock, rfds);
    *max_fd = *srv_sock;
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (clt_sock[i] != -1) {
            printf("%d: add %d\n", i, clt_sock[i]);
            FD_SET(clt_sock[i], rfds);
            *max_fd = clt_sock[i] > *max_fd ? clt_sock[i] : *max_fd;
        }
        else {
            *num = i;
        }
    }
}

int accept_client(fd_set* rfds, int* srv_sock, int* clt_sock, int* num, struct sockaddr_in* clt_sockaddr, socklen_t* clt_sockaddr_len) {
    if (FD_ISSET(*srv_sock, rfds)) {
        printf("ready to accept\n");
        memset(&clt_sockaddr[*num], 0, sizeof(struct sockaddr_in));
        clt_sock[*num] = accept(*srv_sock, (struct sockaddr*) &clt_sockaddr[*num], clt_sockaddr_len);
        if (clt_sock[*num] == FUNC_ERROR) {
            return ACCEPT_ERROR;
        }
    }
    return FUNC_SUCCESS;
}

int read_client(fd_set* rfds, int* clt_sock, struct sockaddr_in* clt_sockaddr, char* buff, int buff_size) {
    int res;
    int recv_len;
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (FD_ISSET(clt_sock[i], rfds)) {
            memset(buff, 0, buff_size);
            recv_len = read(clt_sock[i], buff, buff_size);
            if (recv_len == FUNC_ERROR) {
                return READ_ERROR;
            }
            if (recv_len == 0) {
                printf("interrupted connection\n");
                close(clt_sock[i]);
                clt_sock[i] = -1;
                continue;
            }
            printf("Received from %s:%d\n", inet_ntoa(clt_sockaddr[i].sin_addr), ntohs(clt_sockaddr[i].sin_port));
            printf("%s\n", buff);
            res = write(clt_sock[i], buff, recv_len);
            if (res == FUNC_ERROR) {
                return WRITE_ERROR;
            }
        }
    }
    return FUNC_SUCCESS;
}

int main(int argc, char* argv[]) {
    char* ip = IP;
    int port = PORT;
    if (argc == 3) {
        ip = argv[1];
        port = atoi(argv[2]);
    }
	struct sockaddr_in srv_sockaddr;
    struct sockaddr_in clt_sockaddr[MAX_CLIENT];
	int srv_sock;
    int clt_sock[MAX_CLIENT];
    for (int i = 0; i < MAX_CLIENT; i++) {
        clt_sock[i] = -1;
    }
	int res, func_res;
    int max_fd;
    int num = 0;
    fd_set rfds;
    struct timeval tv;
    socklen_t clt_sockaddr_len = sizeof(clt_sockaddr);
	char buff[BUFF_SIZE];
    signal(SIGPIPE, SIG_IGN);
	
	srv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (srv_sock == FUNC_ERROR) {
        printf("socket error\n");
        return PROG_FAILED;
    }
	
	memset((char*)&srv_sockaddr, 0, sizeof(struct sockaddr_in));
	srv_sockaddr.sin_family = AF_INET;
	srv_sockaddr.sin_port = htons(port);// the unsigned short integer hostshort from host byte order to network byte order
	
    res = inet_aton(ip, &srv_sockaddr.sin_addr);
	if (res == 0) {
		printf("inet_aton error\n");
        res = close(srv_sock);
        return PROG_FAILED;
	}
	
	res = bind(srv_sock, (struct sockaddr*) &srv_sockaddr, sizeof(srv_sockaddr));
    if (res == FUNC_ERROR) {
        printf("bind error\n");
        res = close(srv_sock);
        return PROG_FAILED;
    }

    res = listen(srv_sock, MAX_CLIENT);
    if (res == FUNC_ERROR) {
        printf("listen error\n");
        res = close(srv_sock);
        return PROG_FAILED;
    }

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    printf("waiting connections\n");

	while(1) {
        prepare_select(&max_fd, &num, &rfds, &srv_sock, clt_sock);
        
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        
        res = select(max_fd + 1, &rfds, NULL, NULL, &tv);
        if (res == 0) {
            printf("no activity on the socets for 5 sec\n");
            continue;
        }
        else if (res > 0) {
            func_res  = accept_client(&rfds, &srv_sock, clt_sock, &num, clt_sockaddr, &clt_sockaddr_len);
            if (func_res != FUNC_SUCCESS) {
                break;
            }
            func_res = read_client(&rfds, clt_sock, clt_sockaddr, buff, sizeof(buff));
            if (func_res != FUNC_SUCCESS) {
                break;
            }
        }
        else {
            func_res = SELECT_ERROR;
            break;
        }
    }

    for (int i = 0; i < MAX_CLIENT; i++) {
        if (clt_sock[i] != -1) {
            res = close(clt_sock[i]);
            if (res != PROG_SUCCESS) {
                printf("close error\n");
                return PROG_FAILED;
            }
        }
    }
    res = close(srv_sock);
    if (res != PROG_SUCCESS) {
        printf("close error\n");
        return PROG_FAILED;
    }

    if (func_res == ACCEPT_ERROR) {
        printf("accept error\n");
        return PROG_FAILED;
    }
    if (func_res == READ_ERROR) {
        printf("read error\n");
        return PROG_FAILED;
    }
    if (func_res == WRITE_ERROR) {
        printf("write error\n");
        return PROG_FAILED;
    }
    if (func_res == SELECT_ERROR) {
        printf("select error\n");
        return PROG_FAILED;
    }
    return PROG_SUCCESS;
}