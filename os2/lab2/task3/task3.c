#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

#define PROG_FAILED 1
#define PROG_SUCCESS 0
#define FUNC_ERROR -1
#define FUNC_SUCCESS 0

#define STORAGE_LEN 10000

#define SPINLOCK 1
#define MUTEX 2

// #define version SPINLOCK
#define version MUTEX

int res1 = 0;
int res2 = 0;
int res3 = 0;
int counter = 0;

#if version == MUTEX
    pthread_mutex_t sync_counter;

    typedef struct _Node {
        char value[100];
        struct _Node* next;
        pthread_mutex_t sync;
    } Node;

    typedef struct _Storage {
        Node *first;
        pthread_mutex_t sync;
    } Storage;


    void *first_routine(void *arg) {
        Storage *s = arg;
        Node *last = s->first;
        int last_len = strlen(last->value);
        int cur_len;
        Node *cur = last->next;

        int count = 0;

        while (cur != NULL) {
            cur_len = strlen(cur->value);
            if (cur_len > last_len) {
                count++;
            }
            last = cur;
            cur_len = last_len;
            cur = cur->next;
        }

        res1 += count;

        return NULL;
    }

    void *second_routine(void *arg) {
        Storage *s = arg;
        Node *last = s->first;
        int last_len = strlen(last->value);
        int cur_len;
        Node *cur = last->next;

        int count = 0;

        while (cur != NULL) {
            cur_len = strlen(cur->value);
            if (cur_len < last_len) {
                count++;
            }
            last = cur;
            cur_len = last_len;
            cur = cur->next;
        }

        res2 += count;

        return NULL;
    }

    void *third_routine(void *arg) {
        Storage *s = arg;
        Node *last = s->first;
        int last_len = strlen(last->value);
        int cur_len;
        Node *cur = last->next;

        int count = 0;

        while (cur != NULL) {
            cur_len = strlen(cur->value);
            if (cur_len == last_len) {
                count++;
            }
            last = cur;
            cur_len = last_len;
            cur = cur->next;
        }

        res3 += count;

        return NULL;
    }

    void change_nodes(Node *last, Node *n, Node *next) {
        // printf("tid = %d change\n", gettid());
        if (last != NULL) {
            last->next = next;
        }
        n->next = next->next;
        next->next = n;
    }

    void *change_routine(void *arg) {
        int err;
        Storage *s = arg;
        Node *n, *next;
        Node *last = NULL;
        int flag;

        while (1) {
            flag = 1;
            err = pthread_mutex_lock(&s->sync);
            if (err != FUNC_SUCCESS) {
                printf("error\n");
                abort();
            }

            n = s->first;
            err = pthread_mutex_lock(&n->sync);
            if (err != FUNC_SUCCESS) {
                printf("error\n");
                abort();
            }

            next = n->next;
            err = pthread_mutex_lock(&next->sync);
            if (err != FUNC_SUCCESS) {
                printf("error\n");
                abort();
            }

            if (rand() % 2) {
                change_nodes(last, n, next);
                err = pthread_mutex_lock(&sync_counter);
                if (err != FUNC_SUCCESS) {
                    printf("error\n");
                    abort();
                }
                counter++;
                err = pthread_mutex_unlock(&sync_counter);
                if (err != FUNC_SUCCESS) {
                    printf("error\n");
                    abort();
                }
                n = next;
                s->first = n;
            }
            err = pthread_mutex_unlock(&s->sync);
            if (err != FUNC_SUCCESS) {
                printf("error\n");
                abort();
            }

            last = n;
            n = n->next;
            next = n->next;
            if (next == NULL) {
                flag = 0;
            } else {
                err = pthread_mutex_lock(&next->sync);
                if (err != FUNC_SUCCESS) {
                    printf("error\n");
                    abort();
                }
            }

            while (flag) {
                if (rand() % 2) {
                    change_nodes(last, n, next);
                    err = pthread_mutex_lock(&sync_counter);
                    if (err != FUNC_SUCCESS) {
                        printf("error\n");
                        abort();
                    }
                    counter++;
                    err = pthread_mutex_unlock(&sync_counter);
                    if (err != FUNC_SUCCESS) {
                        printf("error\n");
                        abort();
                    }
                    n = last->next;
                }
                err = pthread_mutex_unlock(&last->sync);
                if (err != FUNC_SUCCESS) {
                    printf("error\n");
                    abort();
                }
                last = n;
                n = n->next;
                next = n->next;
                if (next == NULL) {
                    flag = 0;
                } else {
                    err = pthread_mutex_lock(&next->sync);
                    if (err != FUNC_SUCCESS) {
                        printf("error\n");
                        abort();
                    }
                }
            }
            err = pthread_mutex_unlock(&last->sync);
            if (err != FUNC_SUCCESS) {
                printf("error\n");
                abort();
            }
            err = pthread_mutex_unlock(&n->sync);
            if (err != FUNC_SUCCESS) {
                printf("error\n");
                abort();
            }
        }

        return NULL;
    }

    void *statistic(void *arg) {
        while (1) {
            pthread_mutex_lock(&sync_counter);
            printf("counter = %d\n", counter);
            pthread_mutex_unlock(&sync_counter);
            sleep(1);
        }

        return NULL;
    }

    void make_node(Node *n) {
        int err;
        pthread_mutex_t mutex;
        pthread_mutexattr_t attr;

        memset(n->value, 'a', rand() % 100);

        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
        err = pthread_mutex_init(&mutex, &attr);
        if (err != FUNC_SUCCESS) {
            printf("pthread_mutex_init() failed: %s\n", strerror(err));
            abort();
        }
        n->sync = mutex;
    }

    void storage_init(Storage *s, int len) {
        int err;
        Node *last, *cur;
        
        last = (Node *) malloc(sizeof(Node));
        make_node(last);
        last->next = NULL;
        s->first = last;

        pthread_mutex_t mutex;
        pthread_mutexattr_t attr;
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
        err = pthread_mutex_init(&mutex, &attr);
        if (err != FUNC_SUCCESS) {
            printf("pthread_mutex_init() failed: %s\n", strerror(err));
            abort();
        }
        s->sync = mutex;
        
        for (int i = 0; i < len; i++) {
            cur = (Node *) malloc(sizeof(Node));
            make_node(cur);
            cur->next = NULL;
            last->next = cur;
            last = cur;
        }
    }

#elif version == SPINLOCK
    pthread_spinlock_t sync_counter;

    typedef struct _Node {
        char value[100];
        struct _Node* next;
        pthread_spinlock_t sync;
    } Node;

    typedef struct _Storage {
        Node *first;
        pthread_spinlock_t sync;
    } Storage;


    void *first_routine(void *arg) {
        Storage *s = arg;
        Node *last = s->first;
        int last_len = strlen(last->value);
        int cur_len;
        Node *cur = last->next;

        int count = 0;

        while (cur != NULL) {
            cur_len = strlen(cur->value);
            if (cur_len > last_len) {
                count++;
            }
            last = cur;
            cur_len = last_len;
            cur = cur->next;
        }

        res1 += count;

        return NULL;
    }

    void *second_routine(void *arg) {
        Storage *s = arg;
        Node *last = s->first;
        int last_len = strlen(last->value);
        int cur_len;
        Node *cur = last->next;

        int count = 0;

        while (cur != NULL) {
            cur_len = strlen(cur->value);
            if (cur_len < last_len) {
                count++;
            }
            last = cur;
            cur_len = last_len;
            cur = cur->next;
        }

        res2 += count;

        return NULL;
    }

    void *third_routine(void *arg) {
        Storage *s = arg;
        Node *last = s->first;
        int last_len = strlen(last->value);
        int cur_len;
        Node *cur = last->next;

        int count = 0;

        while (cur != NULL) {
            cur_len = strlen(cur->value);
            if (cur_len == last_len) {
                count++;
            }
            last = cur;
            cur_len = last_len;
            cur = cur->next;
        }

        res3 += count;

        return NULL;
    }

    void change_nodes(Node *last, Node *n, Node *next) {
        // printf("tid = %d change\n", gettid());
        if (last != NULL) {
            last->next = next;
        }
        n->next = next->next;
        next->next = n;
    }

    void *change_routine(void *arg) {
        int err;
        Storage *s = arg;
        Node *n, *next;
        Node *last = NULL;
        int flag;

        while (1) {
            flag = 1;
            err = pthread_spin_lock(&s->sync);
            if (err != FUNC_SUCCESS) {
                printf("error\n");
                abort();
            }

            n = s->first;
            err = pthread_spin_lock(&n->sync);
            if (err != FUNC_SUCCESS) {
                printf("error\n");
                abort();
            }

            next = n->next;
            err = pthread_spin_lock(&next->sync);
            if (err != FUNC_SUCCESS) {
                printf("error\n");
                abort();
            }

            if (rand() % 2) {
                change_nodes(last, n, next);
                err = pthread_spin_lock(&sync_counter);
                if (err != FUNC_SUCCESS) {
                    printf("error\n");
                    abort();
                }
                counter++;
                err = pthread_spin_unlock(&sync_counter);
                if (err != FUNC_SUCCESS) {
                    printf("error\n");
                    abort();
                }
                n = next;
                s->first = n;
            }
            err = pthread_spin_unlock(&s->sync);
            if (err != FUNC_SUCCESS) {
                printf("error\n");
                abort();
            }

            last = n;
            n = n->next;
            next = n->next;
            if (next == NULL) {
                flag = 0;
            } else {
                err = pthread_spin_lock(&next->sync);
                if (err != FUNC_SUCCESS) {
                    printf("error\n");
                    abort();
                }
            }

            while (flag) {
                if (rand() % 2) {
                    change_nodes(last, n, next);
                    err = pthread_spin_lock(&sync_counter);
                    if (err != FUNC_SUCCESS) {
                        printf("error\n");
                        abort();
                    }
                    counter++;
                    err = pthread_spin_unlock(&sync_counter);
                    if (err != FUNC_SUCCESS) {
                        printf("error\n");
                        abort();
                    }
                    n = last->next;
                }
                err = pthread_spin_unlock(&last->sync);
                if (err != FUNC_SUCCESS) {
                    printf("error\n");
                    abort();
                }
                last = n;
                n = n->next;
                next = n->next;
                if (next == NULL) {
                    flag = 0;
                } else {
                    err = pthread_spin_lock(&next->sync);
                    if (err != FUNC_SUCCESS) {
                        printf("error\n");
                        abort();
                    }
                }
            }
            err = pthread_spin_unlock(&last->sync);
            if (err != FUNC_SUCCESS) {
                printf("error\n");
                abort();
            }
            err = pthread_spin_unlock(&n->sync);
            if (err != FUNC_SUCCESS) {
                printf("error\n");
                abort();
            }
        }

        return NULL;
    }

    void *statistic(void *arg) {
        while (1) {
            pthread_spin_lock(&sync_counter);
            printf("counter = %d\n", counter);
            pthread_spin_unlock(&sync_counter);
            sleep(1);
        }

        return NULL;
    }

    void make_node(Node *n) {
        int err;
        pthread_spinlock_t spin;

        memset(n->value, 'a', rand() % 100);

        err = pthread_spin_init(&spin, 0);
        if (err != FUNC_SUCCESS) {
            printf("pthread_mutex_init() failed: %s\n", strerror(err));
            abort();
        }
        n->sync = spin;
    }

    void storage_init(Storage *s, int len) {
        int err;
        Node *last, *cur;
        
        last = (Node *) malloc(sizeof(Node));
        make_node(last);
        last->next = NULL;
        s->first = last;

        pthread_spinlock_t spin;
        err = pthread_spin_init(&spin, 0);
        if (err != FUNC_SUCCESS) {
            printf("pthread_mutex_init() failed: %s\n", strerror(err));
            abort();
        }
        s->sync = spin;
        
        for (int i = 0; i < len; i++) {
            cur = (Node *) malloc(sizeof(Node));
            make_node(cur);
            cur->next = NULL;
            last->next = cur;
            last = cur;
        }
    }
#endif

int main() {
    int err;
    pthread_t tid[4];

    srand(time(NULL));

    Storage *s = (Storage *) malloc(sizeof(Storage));
    int len = STORAGE_LEN;
    storage_init(s, len);

    err = pthread_create(&tid[0], NULL, first_routine, s);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return FUNC_ERROR;
	}
    err = pthread_join(tid[0], NULL);
    if (err != FUNC_SUCCESS) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
		return FUNC_ERROR;
    }

    err = pthread_create(&tid[0], NULL, second_routine, s);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return FUNC_ERROR;
	}
    err = pthread_join(tid[0], NULL);
    if (err != FUNC_SUCCESS) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
		return FUNC_ERROR;
    }

    err = pthread_create(&tid[0], NULL, third_routine, s);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return FUNC_ERROR;
	}
    err = pthread_join(tid[0], NULL);
    if (err != FUNC_SUCCESS) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
		return FUNC_ERROR;
    }

    printf("res1 = %d, res2 = %d, res3 = %d\n", res1, res2, res3);

    #if version == MUTEX
    pthread_mutex_init(&sync_counter, NULL);
    #elif version == SPINLOCK
    pthread_spin_init(&sync_counter, 0);
    #endif

    err = pthread_create(&tid[3], NULL, statistic, NULL);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return FUNC_ERROR;
	}

    for (int i = 0; i < 3; i++) {
        err = pthread_create(&tid[i], NULL, change_routine, s);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            return FUNC_ERROR;
        }
    }

    for (int i = 0; i < 4; i++) {
        err = pthread_join(tid[i], NULL);
        if (err != FUNC_SUCCESS) {
            printf("main: pthread_join() failed: %s\n", strerror(err));
            return FUNC_ERROR;
        }
    }

    return PROG_SUCCESS;
}