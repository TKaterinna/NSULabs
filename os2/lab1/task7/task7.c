#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "uthread_lib.h"

#define PROG_FAILED 1
#define PROG_SUCCESS 0
#define FUNC_ERROR -1
#define FUNC_SUCCESS 0


void *routine(void *arg) {
	int i = 0;
	while (i < 2) {
		i++;
		printf("thread [%d %d %d]: Hello from thread! %d\n", getpid(), getppid(), gettid(), atoi((char *)arg));
		sleep(atoi((char *)arg));
		schedule();
	}

	sleep(3);
	
	return NULL;
}

int main() {
	uthread_t *threads[3];
	int err;

	uthread_t main_thread;
	err = getcontext(&main_thread.context);
    if (err == FUNC_ERROR) {
        printf("getcontext failed: %s\n", strerror(err));
		return PROG_FAILED;
    }
	setmain(main_thread);

	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

	err = uthread_create(&threads[0], routine, (void *)"1");
	if (err == FUNC_ERROR) {
        printf("uthread_create failed: %s\n", strerror(err));
		return PROG_FAILED;
    }
	err = uthread_create(&threads[1], routine, (void *)"2");
	if (err == FUNC_ERROR) {
        printf("uthread_create failed: %s\n", strerror(err));
		return PROG_FAILED;
    }
	err = uthread_create(&threads[2], routine, (void *)"3");
	if (err == FUNC_ERROR) {
        printf("uthread_create failed: %s\n", strerror(err));
		return PROG_FAILED;
    }

	while (1) {
		printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());
		schedule();
	}

    printf("end\n");

	return PROG_SUCCESS;
}