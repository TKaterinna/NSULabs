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

void *res = 0;

void cleaner(void *buffer) {
    free(buffer);
}

void *mythread(void *arg) {
    int count = 0;
    int err;

    // char *message = (char *)malloc(sizeof(hello_str));
    // if (message == NULL) {
    //     return NULL;
    // }
    // res = (void *)message;
    // printf("thread  %p\n", res);
    // pthread_cleanup_push(cleaner, message);
    // memcpy(message, hello_str, sizeof(hello_str));

    // err = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    // if (err) {
	//     printf("mythread: pthread_setcanceltype() failed: %s\n", strerror(err));
	// 	return NULL;
	// }

    while (1) {
	    printf("mythread [%d %d %d %ld]: Hello from mythread!\n", getpid(), getppid(), gettid(), pthread_self());

        // count++;
        // pthread_testcancel();

        // printf("mythread: %s\n", message);
    }

    // pthread_cleanup_pop(1);
	
	return NULL;
}

int main() {
	pthread_t tid;
	int err;


	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

	err = pthread_create(&tid, NULL, mythread, NULL);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return PROG_FAILED;
	}

    sleep(1);

	err = pthread_cancel(tid);
	if (err) {
		printf("main: pthread_cancel() failed: %s\n", strerror(err));
		return PROG_FAILED;
	}

    err = pthread_join(tid, NULL);
	if (err) {
		printf("main: pthread_join() failed: %s\n", strerror(err));
		return PROG_FAILED;
	}

    // char *str_res = (char *)res;
    // printf("thread return %p\n", str_res);
    // printf("thread return %s\n", str_res);

	return PROG_SUCCESS;
}