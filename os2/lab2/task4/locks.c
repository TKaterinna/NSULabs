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

#include "locks.h"

static int futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3) {
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

void spinlock_init(spinlock_t *s) {
    s->lock = 1;
}

void spinlock_lock(spinlock_t *s) {
    while (1) {
        int compare = 1;
        if (atomic_compare_exchange_strong(&s->lock, &compare, 0))
            break;
    }
}

void spinlock_unlock(spinlock_t *s) {
    const int compare = 0;
    atomic_compare_exchange_strong(&s->lock, &compare, 1);
}

void mutex_init(mutex_t *m) {
    m->lock = 1;
    m->owner = 0;
}

void mutex_lock(mutex_t *m) {
    int err;
    while (1) {
        int compare = 1;
        
        if (atomic_compare_exchange_strong(&m->lock, &compare, 0)) {
            pid_t compare_id = 0;
            if (!atomic_compare_exchange_strong(&m->owner, &compare_id, gettid())) {
                printf("error %d %d\n", m->owner, gettid());
                abort();
            }
            break;
        }
        err = futex(&m->lock, FUTEX_WAIT, 0, NULL, NULL, 0);
        if (err == FUNC_ERROR && errno != EAGAIN) {
            printf("futex(FUTEX_WAIT) failed: %s\n", strerror(err));
            abort();
        }
    }
}

int mutex_unlock(mutex_t *m) {
    int err;
    const int compare = 0;
    // pid_t owner = m->owner;
    // if (owner != 0 && owner != gettid()) {
    //     printf("you are not owner\n");
    //     return FUNC_ERROR;
    // }

    pid_t owner = gettid();
    if (!atomic_compare_exchange_strong(&m->owner, &owner, 0)) {
        printf("you are not owner\n");
        return FUNC_ERROR;
    } else {
        if (atomic_compare_exchange_strong(&m->lock, &compare, 1)) {
            err = futex(&m->lock, FUTEX_WAKE, 1, NULL, NULL, 0);
            if (err == FUNC_ERROR) {
                printf("futex(FUTEX_WAKE) failed: %s\n", strerror(err));
                abort();
            }
        }
    }

    // if (atomic_compare_exchange_strong(&m->lock, &compare, 1)) {
    //     owner = gettid();
    //     if (!atomic_compare_exchange_strong(&m->owner, &owner, 0)) {
    //         printf("error 2\n");
    //         abort();
    //     }
    //     printf("%d %d %d\n", m->owner, gettid(), owner);
    //     err = futex(&m->lock, FUTEX_WAKE, 1, NULL, NULL, 0);
    //     if (err == FUNC_ERROR) {
    //         printf("futex(FUTEX_WAKE) failed: %s\n", strerror(err));
    //         abort();
    //     }
    // } else {
    //     // owner = gettid();
    //     // if (!atomic_compare_exchange_strong(&m->owner, &owner, 0)) {
    //     //     printf("error 22\n");
    //     //     abort();
    //     // }
    //     m->owner = 0;
    // }
    return FUNC_SUCCESS;
}