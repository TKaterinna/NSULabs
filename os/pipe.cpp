#include <stdio.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define PROG_SUCCESS 0
#define PROG_FAILED 1
#define FUNC_ERROR -1

int main() {
    int pid;
    int pfd[2];
    int err;

    err = pipe(pfd);
    if (err == FUNC_ERROR) {
        printf("pipe error\n");
        return PROG_FAILED;
    }

    bool isfirst = true;
    int ret;
    ssize_t ret_read_write;
    pid = fork();
    if (pid == 0) { //child read
        unsigned int value = 0;
        unsigned int prev_value = 0;
        ret = close(pfd[1]);
        if (ret == FUNC_ERROR) {
            printf("close pfd[1] error\n");
        }
        while (1) {
            ret_read_write = read(pfd[0], &value, sizeof(value));
            if (ret_read_write == FUNC_ERROR) {
                printf("read error\n");
                return PROG_FAILED;
            }
            printf("%u ", value);
            ret = fflush(stdout);
            if (ret == EOF) {
                printf("fflush error\n");
                return PROG_FAILED;
            }
            if (!isfirst && prev_value + 1 != value) {
                printf("error\n");
                return PROG_SUCCESS;
            }
            isfirst = false;
            prev_value = value;
        }
        ret = close(pfd[0]);
        if (ret == FUNC_ERROR) {
            printf("close pfd[0] error\n");
        }
    }
    else if (pid > 0) { //parent write
        unsigned int value = 0;
        ret = close(pfd[0]);
        if (ret == FUNC_ERROR) {
            printf("close pfd[0] error\n");
        }
        // unsigned int ret_sleep;
        while (1) {
            ret_read_write = write(pfd[1], &value, sizeof(value));
            if (ret_read_write == FUNC_ERROR) {
                printf("write error\n");
                return PROG_FAILED;
            }
            value++;
            // ret_sleep = sleep(1);
            // if (ret_sleep != PROG_SUCCESS) {
            //     perror("interrupted sleep");
            //     return PROG_FAILED;
            // }
        }
        ret = close(pfd[1]);
        if (ret == FUNC_ERROR) {
            printf("close pfd[1] error\n");
        }
    }

    return PROG_SUCCESS;
}