#include <stdio.h>
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

int main() {
	struct sockaddr_in srv_sockaddr, clt_sockaddr;
	
	int sock, res;
    int recv_len = BUFF_SIZE;
    socklen_t clt_sockaddr_len = sizeof(clt_sockaddr);
	char buff[BUFF_SIZE];
	
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == FUNC_ERROR) {
        printf("socket error\n");
        return PROG_FAILED;
    }
	
	memset((char*)&srv_sockaddr, 0, sizeof(struct sockaddr_in));
	srv_sockaddr.sin_family = AF_INET;
	srv_sockaddr.sin_port = htons(PORT);// the unsigned short integer hostshort from host byte order to network byte order
	
    // srv_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); //the unsigned integer hostlong from host byte order to network byte order
                                                         //INADDR_ANY 0.0.0.0 т.е. любой локальный адрес
    res = inet_aton(IP, &srv_sockaddr.sin_addr); // the Internet host address cp from the IPv4  numbers-and-dots  notation  into binary form (in network byte order) and stores it in the structure that inp points to
	if (res == 0) {
		printf("inet_aton error\n");
        res = close(sock);
        return PROG_FAILED;
	}
	
	res = bind(sock, (struct sockaddr*) &srv_sockaddr, sizeof(srv_sockaddr));
    if (res == FUNC_ERROR) {
        printf("bind error\n");
        res = close(sock);
        return PROG_FAILED;
    }
	
    printf("waiting for data\n");

	while(1) {
        memset(buff, 0, recv_len);
		recv_len = recvfrom(sock, buff, sizeof(buff), 0, (struct sockaddr*) &clt_sockaddr, &clt_sockaddr_len);
		if (recv_len == FUNC_ERROR) {
			printf("recvfrom error\n");
            res = close(sock);
            return PROG_FAILED;
		}
		if (recv_len != 0) {
            printf("Received from %s:%d\n", inet_ntoa(clt_sockaddr.sin_addr), ntohs(clt_sockaddr.sin_port)); 
            //converts the Internet host address in, given in network byte order, to a string in IPv4 dotted-decimal notation
            //ntohs - the unsigned short integer netshort from network byte order to host byte order//пояснение записать и вообще зачем
		    printf("%s\n" , buff);
        }
		res = sendto(sock, buff, recv_len, 0, (struct sockaddr*) &clt_sockaddr, clt_sockaddr_len);
		if (res == FUNC_ERROR) {
            printf("sendto error\n");
            res = close(sock);
            return PROG_FAILED;
        }
	}

	res = close(sock);
    if (res != PROG_SUCCESS) {
        printf("close error\n");
        return PROG_FAILED;
    }

    return PROG_SUCCESS;
}