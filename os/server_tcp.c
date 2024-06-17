#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h> //sockaddr_in
#include <unistd.h>
#include <arpa/inet.h> //inet_ntoa
#include <wait.h>

#define PROG_SUCCESS 0
#define PROG_FAILED 1
#define FUNC_ERROR -1
#define BUFF_SIZE 1024
#define MAX_CLIENT 5
#define PORT 8888
#define IP "127.0.0.1"

void sigchld_handler(int signum) {
    printf("Caught SIGCHILD\n");
    pid_t pid = waitpid(-1, NULL, 0);
    if (pid == FUNC_ERROR) {
        printf("waitpid error\n");
        exit(PROG_FAILED);
    }
}

int main(int argc, char* argv[]) {
    char* ip = IP;
    int port = PORT;
    if (argc == 3) {
        ip = argv[1];
        port = atoi(argv[2]);
    }
	struct sockaddr_in srv_sockaddr, clt_sockaddr;
	int srv_sock;
    int clt_sock;
	int res;
    pid_t pid;
    socklen_t clt_sockaddr_len = sizeof(clt_sockaddr);
	char buff[BUFF_SIZE];
    int recv_len = sizeof(buff);
    // signal(SIGPIPE, SIG_IGN);

    signal(SIGCHLD, sigchld_handler);
	
	srv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (srv_sock == FUNC_ERROR) {
        printf("socket error\n");
        return PROG_FAILED;
    }
	
	memset((char*)&srv_sockaddr, 0, sizeof(struct sockaddr_in));
	srv_sockaddr.sin_family = AF_INET;
	srv_sockaddr.sin_port = htons(port);// the unsigned short integer hostshort from host byte order to network byte order
	
    // srv_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); //the unsigned integer hostlong from host byte order to network byte order
                                                         //INADDR_ANY 0.0.0.0 т.е. любой локальный адрес
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

    printf("waiting connections\n");
	
    while(1) {
        memset(&clt_sockaddr, 0, sizeof(struct sockaddr_in));
        
        clt_sock = accept(srv_sock, (struct sockaddr*) &clt_sockaddr, (socklen_t*) &clt_sockaddr_len);
        if (clt_sock == FUNC_ERROR) {
            printf("accept error\n");
            res = close(srv_sock);
            return PROG_FAILED;
        }

        pid = fork();
        if (pid == 0) {
            break;
        }
        else if (pid < 0) {
            printf("fork error\n");
            res = close(clt_sock);
            res = close(srv_sock);
            return PROG_FAILED;
        }
        res = close(clt_sock);
        if (res != PROG_SUCCESS) {
            printf("close error\n");
            return PROG_FAILED;
        }
        
    }

    res = close(srv_sock);
    if (res != PROG_SUCCESS) {
        printf("close error\n");
        return PROG_FAILED;
    }
    printf("waiting for data\n");

	while(1) {
        memset(buff, 0, recv_len);
        recv_len = read(clt_sock, buff, sizeof(buff));
        if (recv_len == FUNC_ERROR || recv_len == 0) {
            printf("read error\n");
            return PROG_FAILED;
        }
        printf("Received from %s:%d\n", inet_ntoa(clt_sockaddr.sin_addr), ntohs(clt_sockaddr.sin_port));
        printf("%s\n", buff);
        res = write(clt_sock, buff, recv_len);
        if (res == FUNC_ERROR) {
            printf("write error\n");
            return PROG_FAILED;
        }

        // memset(buff, 0, recv_len);//чтобы зачистить прошлое сообщение
		// recv_len = recvfrom(srv_sock, buff, BUFF_SIZE, 0, (struct sockaddr*) &clt_sockaddr, &clt_sockaddr_len);
		// if (recv_len == FUNC_ERROR || recv_len == 0) {
		// 	printf("recvfrom error\n");
        //     res = close(srv_sock);
        //     return PROG_FAILED;
		// }
        // printf("Received from %s:%d\n", inet_ntoa(clt_sockaddr.sin_addr), ntohs(clt_sockaddr.sin_port)); 
        // //converts the Internet host address in, given in network byte order, to a string in IPv4 dotted-decimal notation
        // //ntohs - the unsigned short integer netshort from network byte order to host byte order//пояснение записать и вообще зачем
		// printf("%s\n" , buff);
		// res = sendto(srv_sock, buff, recv_len, 0, (struct sockaddr*) &clt_sockaddr, clt_sockaddr_len);
		// if (res == FUNC_ERROR) {
        //     printf("sendto error\n");
        //     res = close(srv_sock);
        //     return PROG_FAILED;
        // }
	}

	res = close(clt_sock);
    if (res != PROG_SUCCESS) {
        printf("close error\n");
        return PROG_FAILED;
    }

    return PROG_SUCCESS;
}