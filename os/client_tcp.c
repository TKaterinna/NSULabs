#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h> //sockaddr_in
#include <unistd.h>
#include <arpa/inet.h> //inet_ntoa

#define PROG_SUCCESS 0
#define PROG_FAILED 1
#define FUNC_ERROR -1
#define BUFF_SIZE 1024
#define PORT 8888
#define IP "127.0.0.1" 

int main(int argc, char* argv[]) {
    int sock;
    struct sockaddr_in srv_sockaddr;
	int res;
	char buff[BUFF_SIZE];
    char* res_fgets;

    char* ip = IP;
    int port = PORT;
    if (argc == 3) {
        ip = argv[1];
        port = atoi(argv[2]);
    }

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == FUNC_ERROR) {
        printf("socket error\n");
        return PROG_FAILED;
    }

	memset((char*) &srv_sockaddr, 0, sizeof(struct sockaddr_in));
	srv_sockaddr.sin_family = AF_INET;
	srv_sockaddr.sin_port = htons(port);// the unsigned short integer hostshort from host byte order to network byte order
	
    res = inet_aton(ip, &srv_sockaddr.sin_addr); //converts the Internet host address cp from the IPv4 numbers-and-dots notation into binary form (in network byte order) and stores it in the structure that inp points to
	if (res == 0) {
		printf("inet_aton error\n");
        res = close(sock);
        return PROG_FAILED;
	}

    res = connect(sock, (struct sockaddr*) &srv_sockaddr, sizeof(srv_sockaddr));
    if (res == FUNC_ERROR) {
        printf("connect error\n");
        res = close(sock);
        return PROG_FAILED;
    }

	while(1) {
        res_fgets = fgets(buff, sizeof(buff), stdin);
        if (res_fgets == NULL) {
            printf("fgets error\n");
            res = close(sock);
            return PROG_FAILED;
        }
        res = write(sock, buff, sizeof(buff));
        if (res == FUNC_ERROR) {
            printf("write error\n");
            continue;
        }
        res = read(sock, buff, sizeof(buff));
        if (res == FUNC_ERROR) {
            printf("read error\n");
            continue;
        }
        if (res != 0) {
            printf("%s\n" , buff);
        }
	}

	res = close(sock);
    if (res != PROG_SUCCESS) {
        printf("close error\n");
        return PROG_FAILED;
    }

    return PROG_SUCCESS;
}