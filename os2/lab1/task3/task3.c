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
	printf("mythread [%d %ld]: id = %d, message = %s\n", gettid(), pthread_self(), ((arg_st *)arg)->id, ((arg_st *)arg)->message);
	
	return NULL;
}

int main() {
	pthread_t tid;
	int err;

	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    // arg_st *thread_arg = (arg_st *)malloc(sizeof(arg_st));
    // char *message = (char *)malloc(sizeof(hello_str));
    // memcpy(message, hello_str, sizeof(hello_str));
    // thread_arg->id = 1;
    // thread_arg->message = message;

    arg_st thread_arg;
    thread_arg.id = 1;
    char *message = hello_str;
    thread_arg.message = message;

    // err = pthread_create(&tid, NULL, mythread, (void *)thread_arg);
    err = pthread_create(&tid, NULL, mythread, (void *)&thread_arg);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return PROG_FAILED;
    }

    err = pthread_join(tid, NULL);
    if (err) {
		printf("main: pthread_join() failed: %s\n", strerror(err));
		return PROG_FAILED;
	}

    // free(thread_arg->message);
    // free(thread_arg);

	return PROG_SUCCESS;
}