#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/un.h> //sockaddr_un
#include <unistd.h>
#include <wait.h>

#define PROG_SUCCESS 0
#define PROG_FAILED 1
#define FUNC_ERROR -1
#define MAX_CLIENT 1
#define BUFF_SIZE 8

void sigchld_handler(int signum) {
    printf("Caught SIGCHILD\n");
    pid_t pid = waitpid(-1, NULL, 0);
    if (pid == FUNC_ERROR) {
        printf("waitpid error\n");
        exit(PROG_FAILED);
    }
}

int main() {
    int srv_sock;
    int clt_sock;
    struct sockaddr_un srv_sockaddr;
    struct sockaddr_un clt_sockaddr;
    int clt_sockaddr_len = 0;
    pid_t pid;
    int res;
    unsigned int ret_sleep;

    char* dsock_file = "./dsock_file";

    signal(SIGCHLD, sigchld_handler);

    srv_sock = socket(AF_UNIX, SOCK_STREAM, 0); //AF_UNIX - Local communication; SOCK_STREAM - Provides  sequenced,  reliable,  two-way,  connection-based byte streams.  An out-of-band data transmission mechanism may be supported.
    if (srv_sock == FUNC_ERROR) {
        printf("socket error\n");
        return PROG_FAILED;
    }

    memset(&srv_sockaddr, 0, sizeof(struct sockaddr_un));
    srv_sockaddr.sun_family = AF_UNIX;
    strcpy(srv_sockaddr.sun_path, dsock_file);

    res = unlink(dsock_file);
    if (res == FUNC_ERROR) {
        printf("unlink error\n");
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

    clt_sockaddr_len = sizeof(clt_sockaddr);

    while(1) {
        memset(&clt_sockaddr, 0, sizeof(struct sockaddr_un));
        
        clt_sock = accept(srv_sock, (struct sockaddr*) &clt_sockaddr, (socklen_t*) &clt_sockaddr_len);
        if (clt_sock == FUNC_ERROR) {
            printf("accept error\n");
            res = close(srv_sock);
            return PROG_FAILED;
        }
        
        printf("Client socket filepath: %s\n", clt_sockaddr.sun_path);

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
            res = close(srv_sock);
            return PROG_FAILED;
        }
        
    }

    res = close(srv_sock);
    if (res != PROG_SUCCESS) {
        printf("close error\n");
        return PROG_FAILED;
    }

    char buff[BUFF_SIZE];
    while(1) {
        res = read(clt_sock, buff, sizeof(buff));
        if (res == FUNC_ERROR || res == 0) {
            printf("read error\n");
            res = close(clt_sock);
            return PROG_FAILED;
        }
        printf("%s\n", buff);
        ret_sleep = sleep(1);
        if (ret_sleep != PROG_SUCCESS) {
            printf("interrupted sleep\n");
            res = close(clt_sock);
            return PROG_FAILED;
        }
        res = write(clt_sock, buff, sizeof(buff));
        if (res == FUNC_ERROR) {
            printf("write error\n");
            res = close(clt_sock);
            return PROG_FAILED;
        }
    }

    res = close(clt_sock);
    if (res != PROG_SUCCESS) {
        printf("close error\n");
        return PROG_FAILED;
    }

    return PROG_SUCCESS;
}