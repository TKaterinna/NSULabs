#define GNU_SOURCE
#include <sys/stat.h>
#include <fcntl.h>
#include <stdatomic.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <stdlib.h>
#include <stdio.h>

#define PROG_FAILED 1
#define PROG_SUCCESS 0
#define FUNC_ERROR -1
#define FUNC_SUCCESS 0

typedef struct {
    int lock;
} spinlock_t;

typedef struct {
    int lock;
    pid_t owner;
} mutex_t;

void spinlock_init(spinlock_t *s);
void spinlock_lock(spinlock_t *s);
void spinlock_unlock(spinlock_t *s);

void mutex_init(mutex_t *m);
void mutex_lock(mutex_t *m);
int mutex_unlock(mutex_t *m);