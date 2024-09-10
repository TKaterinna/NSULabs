#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#define hello_str "hello world"
#define PROG_FAILED 1
#define PROG_SUCCESS 0

typedef struct {
    int id;
    char *message;
} arg_st;

void *mythread(void *arg) {
    arg_st *my_arg = (arg_st *)arg;
    printf("mythread [%d %ld]: id = %d, message = %s\n", gettid(), pthread_self(), my_arg->id, my_arg->message);

    free(my_arg->message);
    free(my_arg);

	return NULL;
}

int main() {
	pthread_t tid;
	int err;
    pthread_attr_t attr;

	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    arg_st *thread_arg = (arg_st *)malloc(sizeof(arg_st));
    char *message = (char *)malloc(sizeof(hello_str));
    memcpy(message, hello_str, sizeof(hello_str));
    thread_arg->id = 1;
    thread_arg->message = message;

    err = pthread_attr_init(&attr);
    if (err) {
        printf("main: pthread_attr_init() failed: %s\n", strerror(err));
        return PROG_FAILED;
    }

    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (err) {
        printf("main: pthread_attr_setdetachstate() failed: %s\n", strerror(err));
        return PROG_FAILED;
    }

    err = pthread_create(&tid, &attr, mythread, (void *)thread_arg);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return PROG_FAILED;
    }

    err = pthread_attr_destroy(&attr);
    if (err) {
        printf("main: pthread_attr_destroy() failed: %s\n", strerror(err));
        return PROG_FAILED;
    }

    pthread_exit(PROG_SUCCESS);
	// return PROG_SUCCESS;
}