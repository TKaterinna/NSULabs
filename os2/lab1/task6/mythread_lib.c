#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <sched.h>
#include <sys/wait.h>

#include "mythread_lib.h"

#define PROG_FAILED 1
#define PROG_SUCCESS 0
#define FUNC_ERROR -1
#define FUNC_SUCCESS 0

#define PAGE_SIZE 4096
#define STACK_SIZE PAGE_SIZE * 16
#define JOINSLEEP_TIME 3 

void *create_stack(size_t size) {
    void *stack;

    stack = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    // stack = mmap(NULL, size + PAGE_SIZE, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (stack == MAP_FAILED) {
        printf("mmap() failed\n");
		return NULL;
    }
    // mprotect(stack + PAGE_SIZE, STACK_SIZE - PAGE_SIZE, PROT_READ|PROT_WRITE);

    printf("stack addr = %p\n", stack);
    return stack;
}

int start_thread(void *thread_st) {
    void *res;

    mythread_t *thread = (mythread_t *)thread_st;
    thread->tid - gettid();
    res = thread->routine(thread->arg);
    thread->res = res;
    thread->exited = 1;

    while (!thread->joined) {
        sleep(JOINSLEEP_TIME);
    }

    return FUNC_SUCCESS;
}

int mythread_create(mythread_t *thread, func start_routine, void *arg) {
    int err;
    void *stack;

    thread->routine = (func) start_routine;
    thread->arg = arg;
    thread->res = NULL;
    thread->joined = 0;
    thread->exited = 0;
    
    stack = create_stack(STACK_SIZE);
    thread->stack = stack;

    err = clone(start_thread, stack + STACK_SIZE, CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_THREAD|CLONE_SYSVSEM, (void *)thread);
    if (err == FUNC_ERROR) {
	    printf("clone() failed: %s\n", strerror(err));
		return FUNC_ERROR;
	}

    return FUNC_SUCCESS;
}

int mythread_join(mythread_t *thread, void **res) {
    while (!thread->exited) {
        sleep(JOINSLEEP_TIME);
    }
    if (res) {
        *res = thread->res;
    }
    thread->joined = 1;

    wait(&thread->tid);
    int err = munmap(thread->stack, STACK_SIZE);
    if (err == FUNC_ERROR) {
        printf("munmap() failed: %s\n", strerror(err));
		return FUNC_ERROR;
    }

    return FUNC_SUCCESS;
}