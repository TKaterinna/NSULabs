#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "mythread_lib.h"

#define PROG_FAILED 1
#define PROG_SUCCESS 0
#define FUNC_ERROR -1
#define FUNC_SUCCESS 0


void *routine(void *arg) {
	printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());

	sleep(3);
	
	return NULL;
}

int main() {
	mythread_t tid;
	int err;

	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

	err = mythread_create(&tid, routine, NULL);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return PROG_FAILED;
	}

    err = mythread_join(&tid, NULL);
	sleep(3);

	return PROG_SUCCESS;
}