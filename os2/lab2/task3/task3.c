#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <pthread.h>

#define PROG_FAILED 1
#define PROG_SUCCESS 0
#define FUNC_ERROR -1
#define FUNC_SUCCESS 0

#define STORAGE_LEN 100

int res1 = 0;
int res2 = 0;
int res3 = 0;
int counter = 0;
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

//здесь уж не нужна синхронизация, раз они работают поочереди
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
    printf("tid = %d change\n", gettid());
    if (last != NULL) {
        last->next = next;
    }
    n->next = next->next;
    next->next = n;
}

void *change_routine(void *arg) {
    Storage *s = arg;
    Node *n, *next;
    Node *last = NULL;
    int flag;
    while (1) {
        flag = 1;
        pthread_mutex_lock(&s->sync);

        pthread_mutex_lock(&s->first->sync);

        pthread_mutex_unlock(&s->sync);

        n = s->first;
        pthread_mutex_lock(&n->next->sync);
        if (rand() % 2) {
            next = n->next;
            change_nodes(last, n, next);
            pthread_mutex_lock(&sync_counter);
            counter++;
            pthread_mutex_unlock(&sync_counter);
            n = next;
            s->first = n;
        }
        last = n;
        n = n->next;
        next = n->next;
        if (next == NULL) {
            flag = 0;
        } else {
            pthread_mutex_lock(&next->sync);
        }

        while (flag) {
            if (rand() % 2) {
                change_nodes(last, n, next);
                pthread_mutex_lock(&sync_counter);
                counter++;
                pthread_mutex_unlock(&sync_counter);
                n = last->next;
            }
            pthread_mutex_unlock(&last->sync);
            last = n;
            n = n->next;
            next = n->next;
            if (next == NULL) {
                flag = 0;
            } else {
                pthread_mutex_lock(&next->sync);
            }
        }
        pthread_mutex_unlock(&last->sync);
        pthread_mutex_unlock(&n->sync);
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

    memset(n->value, 'a', rand() % 100);
    err = pthread_mutex_init(&mutex, NULL);
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
    err = pthread_mutex_init(&mutex, NULL);
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

    pthread_mutex_init(&sync_counter, NULL);

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