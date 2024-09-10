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

void *mythread(void *arg) {
    
	printf("mythread [%d %ld]: Hello from mythread!\n", gettid(), pthread_self());
	
	return NULL;
}

int main() {
	pthread_t tid;
	int err;
    pthread_attr_t attr;

	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

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

    while (1) {
        err = pthread_create(&tid, &attr, mythread, NULL);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            return PROG_FAILED;
        }
        // err = pthread_detach(tid);
        // if (err) {
        //     printf("main: pthread_detach() failed: %s\n", strerror(err));
        //     return PROG_FAILED;
        // }

        // pthread_join(tid, NULL);
    }

    // err = pthread_attr_destroy(&attr);
    // if (err) {
    //     printf("main: pthread_attr_destroy() failed: %s\n", strerror(err));
    //     return PROG_FAILED;
    // }

	return PROG_SUCCESS;
}