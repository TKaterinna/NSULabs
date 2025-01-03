#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <sched.h>
#include <sys/wait.h>

#include "uthread_lib.h"

#define PROG_FAILED 1
#define PROG_SUCCESS 0
#define FUNC_ERROR -1
#define FUNC_SUCCESS 0

#define PAGE_SIZE 4096
#define STACK_SIZE PAGE_SIZE * 16

#define MAX_THREADS 10

uthread_t *uthreads[MAX_THREADS];
int uthread_count = 0;
int uthread_cur = 0;

void *create_stack(off_t size) {
    void *stack;

    stack = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK, -1, 0);
    if (stack == MAP_FAILED) {
        printf("mmap() failed\n");
		return NULL;
    }

    printf("stack addr = %p\n", stack);
    return stack;
}

void schedule(void) {
    int err;
    ucontext_t *cur_context, *next_context;

    cur_context = &(uthreads[uthread_cur]->context);
    uthread_cur = (uthread_cur + 1) % uthread_count;
    next_context = &(uthreads[uthread_cur]->context);

    err = swapcontext(cur_context, next_context);
    if (err == FUNC_ERROR) {
        printf("swapcontext failed: %s\n", strerror(err));
		exit(PROG_FAILED);
    }
}

void start_thread(int count) {
    printf("start_thread: i=%d routine: %p; arg: %p\n", count, uthreads[count]->routine, uthreads[count]->arg);
    uthreads[count]->routine(uthreads[count]->arg);
}

void setmain(uthread_t main_thread) {
    uthreads[0] = &main_thread;
    uthread_count = 1;
}

int uthread_create(uthread_t **thread, void *(start_routine), void *arg) {
    char *stack;
    uthread_t *new_thread;
    int err;

    stack = create_stack(STACK_SIZE);
    new_thread = (uthread_t *) (stack + STACK_SIZE - sizeof(uthread_t));

    err = getcontext(&new_thread->context);
    if (err == FUNC_ERROR) {
        printf("getcontext failed: %s\n", strerror(err));
		return FUNC_ERROR;
    }

    new_thread->context.uc_stack.ss_sp = stack;
    new_thread->context.uc_stack.ss_size = STACK_SIZE - sizeof(uthread_t);
    new_thread->context.uc_link = NULL;
    makecontext(&new_thread->context, (void (*)())start_thread, 1, uthread_count);

    new_thread->routine = start_routine;
    new_thread->arg = arg;
    uthreads[uthread_count] = new_thread;
    uthread_count++;
    *thread = new_thread;
    
    return FUNC_SUCCESS;
}