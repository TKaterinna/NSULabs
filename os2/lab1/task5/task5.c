#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#define PROG_FAILED 1
#define PROG_SUCCESS 0

void *mythread1(void *arg) {
    int err;
    sigset_t mask;
    sigfillset(&mask);

    err = pthread_sigmask(SIG_SETMASK, &mask, NULL);
    if (err) {
	    printf("mythread1: pthread_sigmask() failed: %s\n", strerror(err));
		return NULL;
	}

    while (1) {
        printf("1  mythread [%d %d %d %ld]: Hello from mythread1!\n", getpid(), getppid(), gettid(), pthread_self());
        sleep(2);
    }

	return NULL;
}

void sigint_handler(int signum) {
    write(1, "Thread 2 received SIGINT\n", 26);
    pthread_exit(NULL);
    // exit(PROG_FAILED);
}

void *mythread2(void *arg) {
    int err;
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGINT);

    err = pthread_sigmask(SIG_SETMASK, &mask, NULL);
    if (err) {
	    printf("mythread2: pthread_sigmask() failed: %s\n", strerror(err));
		return NULL;
	}

    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    err = sigaction(SIGINT, &sa, NULL);
    if (err == -1) {
	    printf("mythread2: sigaction() failed");
		return NULL;
	}

    while (1) {
        printf("2  mythread [%d %d %d %ld]: Hello from mythread2!\n", getpid(), getppid(), gettid(), pthread_self());
        sleep(2);
    }

	return NULL;
}

void *mythread3(void *arg) {
    int err;
    int sig;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGQUIT);

    err = pthread_sigmask(SIG_SETMASK, &mask, NULL);
    if (err) {
	    printf("mythread3: pthread_sigmask() failed: %s\n", strerror(err));
		return NULL;
	}

    printf("3  mythread [%d %d %d %ld]: Hello from mythread3!\n", getpid(), getppid(), gettid(), pthread_self());

    err = sigwait(&mask, &sig);
    if (err) {
	    printf("mythread3: sigwait() failed: %s\n", strerror(err));
		return NULL;
	}

    if (sig == SIGQUIT) {
        printf("Thread 3 received SIGQUIT\n");
    }

	return NULL;
}

int main() {
	pthread_t tid[3];
	int err;

	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

	err = pthread_create(&tid[0], NULL, mythread1, NULL);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return PROG_FAILED;
	}
    err = pthread_create(&tid[1], NULL, mythread2, NULL);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return PROG_FAILED;
	}
    err = pthread_create(&tid[2], NULL, mythread3, NULL);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return PROG_FAILED;
	}

    for (int i = 0; i < 3; i++) {
        err = pthread_join(tid[i], NULL);
        if (err) {
            printf("main: pthread_join() failed: %s\n", strerror(err));
            return PROG_FAILED;
        }
    }
    

	return PROG_SUCCESS;
}