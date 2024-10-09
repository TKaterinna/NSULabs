#include "locks.h"

static int futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3) {
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr, val3);
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
}

void mutex_lock(mutex_t *m) {
    int err;
    while (1) {
        int compare = 1;
        if (atomic_compare_exchange_strong(&m->lock, &compare, 0))
            break;
        err = futex(&m->lock, FUTEX_WAIT, 0, NULL, NULL, 0);
        if (err == FUNC_ERROR && errno != EAGAIN) {
            printf("futex(FUTEX_WAIT) failed: %s\n", strerror(err));
            abort();
        }
    }
}

void mutex_unlock(mutex_t *m) {
    int err;
    const int compare = 0;
    if (atomic_compare_exchange_strong(&m->lock, &compare, 1)) {
        err = futex(&m->lock, FUTEX_WAKE, 1, NULL, NULL, 0);
        if (err == FUNC_ERROR) {
            printf("futex(FUTEX_WAKE) failed: %s\n", strerror(err));
            abort();
        }
    }
}