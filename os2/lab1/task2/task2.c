#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#define hello_str "hello world"
#define FUNC_ERROR -1
#define FUNC_SUCCESS 0

void *mythread(void *arg) {
    // int* int_ret = (int*) malloc(sizeof(int));
    // *int_ret = 42;

    char *str_ret = (char *)malloc(sizeof(hello_str));
	memcpy(str_ret, hello_str, sizeof(hello_str));


	printf("mythread [%d %d %d %ld]: Hello from mythread!\n", getpid(), getppid(), gettid(), pthread_self());

	sleep(2);

	free(str_ret);
	
	return (void*)str_ret;
}

int main() {
	pthread_t tid;
	int err;
    void *res = 0;

	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

	err = pthread_create(&tid, NULL, mythread, NULL);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return FUNC_ERROR;
	}

	err = pthread_join(tid, &res);
	if (err) {
		printf("main: pthread_join() failed: %s\n", strerror(err));
		return FUNC_ERROR;
	}

    // int *int_res = (int *)res;
    // printf("thread return %d\n", *int_res);

	char *str_res = (char *)res;
    printf("thread return %s\n", str_res);

	return FUNC_SUCCESS;
}