#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>

#define PROG_SUCCESS 0
#define PROG_FAILED 1
#define FUNC_ERROR -1
#define MAX_CLIENT 5
#define BUFF_SIZE 8

int main() {
    int clt_sock;
    struct sockaddr_un srv_sockaddr;
    struct sockaddr_un clt_sockaddr;
    char val = 'a';
    int res;
    unsigned int ret_sleep;

    char* dsock_file = "./dsock_file";
    char* dsock_file_clt = "./dsock_file_clt";
    signal(SIGPIPE, SIG_IGN);

    clt_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (clt_sock == FUNC_ERROR) {
        printf("socket error\n");
        return PROG_FAILED;
    }

    memset(&clt_sockaddr, 0, sizeof(struct sockaddr_un));
    clt_sockaddr.sun_family = AF_UNIX;
    strcpy(clt_sockaddr.sun_path, dsock_file_clt);

    res = unlink(dsock_file_clt);
    if (res == FUNC_ERROR) {
        printf("unlink error\n");
        res = close(clt_sock);
        return PROG_FAILED;
    }
    res = bind(clt_sock, (struct sockaddr *) &clt_sockaddr, sizeof(clt_sockaddr));
    if (res == FUNC_ERROR) {
        printf("bind error\n");
        res = close(clt_sock);
        return PROG_FAILED;
    }

    memset(&srv_sockaddr, 0, sizeof(struct sockaddr_un));
    srv_sockaddr.sun_family = AF_UNIX;
    strcpy(srv_sockaddr.sun_path, dsock_file);

    res = connect(clt_sock, (struct sockaddr*) &srv_sockaddr, sizeof(srv_sockaddr));
    if (res == FUNC_ERROR) {
        printf("connect error\n");
        res = close(clt_sock);
        return PROG_FAILED;
    }

    char buff[BUFF_SIZE];
    while(1) {
        memset(buff, val++, sizeof(buff));
        res = write(clt_sock, buff, sizeof(buff));
        if (res == FUNC_ERROR) {
            printf("write error\n");
            ret_sleep = sleep(1);
            if (ret_sleep != PROG_SUCCESS) {
                printf("interrupted sleep\n");
                res = close(clt_sock);
                return PROG_FAILED;
            }
            continue;
        }
        res = fflush(stdout);
        if (res == EOF) {
            printf("fflush error\n");
            return PROG_FAILED;
        }
        ret_sleep = sleep(1);
        if (ret_sleep != PROG_SUCCESS) {
            printf("interrupted sleep\n");
            res = close(clt_sock);
            return PROG_FAILED;
        }
        res = read(clt_sock, buff, sizeof(buff));
        if (res == FUNC_ERROR) {
            printf("read error\n");
            ret_sleep = sleep(1);
            if (ret_sleep != PROG_SUCCESS) {
                printf("interrupted sleep\n");
                res = close(clt_sock);
                return PROG_FAILED;
            }
            continue;
        }
        if (res != 0) {
            printf("%s\n", buff);
        }
    }

    res = close(clt_sock);
    if (res != PROG_SUCCESS) {
        printf("close error\n");
        return PROG_FAILED;
    }

    return PROG_SUCCESS;
}