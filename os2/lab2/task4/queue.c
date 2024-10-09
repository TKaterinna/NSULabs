#define _GNU_SOURCE
#include <pthread.h>
#include <assert.h>

#include "queue.h"
#include "locks.h"

#define SPINLOCK 1
#define MUTEX 2

// #define version SPINLOCK
#define version MUTEX

#if version == SPINLOCK
	spinlock_t spinlock;

	void *qmonitor(void *arg) {
		queue_t *q = (queue_t *)arg;

		printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

		while (1) {
			queue_print_stats(q);
			sleep(1);
		}

		return NULL;
	}

	queue_t* queue_init(int max_count) {
		int err;

		queue_t *q = malloc(sizeof(queue_t));
		if (!q) {
			printf("Cannot allocate memory for a queue\n");
			abort();
		}

		q->first = NULL;
		q->last = NULL;
		q->max_count = max_count;
		q->count = 0;

		q->add_attempts = q->get_attempts = 0;
		q->add_count = q->get_count = 0;

		spinlock_init(&spinlock);

		err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
		if (err) {
			printf("queue_init: pthread_create() failed: %s\n", strerror(err));
			abort();
		}

		return q;
	}

	void queue_destroy(queue_t *q) {
		int err;
		qnode_t *curr, *next;

		spinlock_lock(&spinlock);

		curr = q->first;
		while (q->count > 0) {
			next = curr->next;
			free(curr);
			curr = next;
			q->count--;
		}
		err = pthread_cancel(q->qmonitor_tid);
		if (err) {
			printf("cannot cancel monitor: %s\n", strerror(err));
		}

		spinlock_unlock(&spinlock);

		free(q);
	}

	int queue_add(queue_t *q, int val) {
		spinlock_lock(&spinlock);

		q->add_attempts++;

		assert(q->count <= q->max_count);

		if (q->count == q->max_count) {
			spinlock_unlock(&spinlock);
			return 0;
		}

		qnode_t *new = malloc(sizeof(qnode_t));
		if (!new) {
			printf("Cannot allocate memory for new node\n");
			abort();
		}

		new->val = val;
		new->next = NULL;

		if (!q->first)
			q->first = q->last = new;
		else {
			q->last->next = new;
			q->last = q->last->next;
		}

		q->count++;
		q->add_count++;

		spinlock_unlock(&spinlock);

		return 1;
	}

	int queue_get(queue_t *q, int *val) {
		spinlock_lock(&spinlock);

		q->get_attempts++;

		assert(q->count >= 0);

		if (q->count == 0) {
			spinlock_unlock(&spinlock);
			return 0;
		}

		qnode_t *tmp = q->first;

		*val = tmp->val;
		q->first = q->first->next;

		free(tmp);
		q->count--;
		q->get_count++;

		spinlock_unlock(&spinlock);

		return 1;
	}

	void queue_print_stats(queue_t *q) {
		spinlock_lock(&spinlock);

		printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
			q->count,
			q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
			q->add_count, q->get_count, q->add_count -q->get_count);

		spinlock_unlock(&spinlock);
	}

#elif version == MUTEX
	mutex_t mutex;

	void *qmonitor(void *arg) {
		queue_t *q = (queue_t *)arg;

		printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

		while (1) {
			queue_print_stats(q);
			sleep(1);
		}

		return NULL;
	}

	queue_t* queue_init(int max_count) {
		int err;

		queue_t *q = malloc(sizeof(queue_t));
		if (!q) {
			printf("Cannot allocate memory for a queue\n");
			abort();
		}

		q->first = NULL;
		q->last = NULL;
		q->max_count = max_count;
		q->count = 0;

		q->add_attempts = q->get_attempts = 0;
		q->add_count = q->get_count = 0;

		mutex_init(&mutex);

		err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
		if (err) {
			printf("queue_init: pthread_create() failed: %s\n", strerror(err));
			abort();
		}

		return q;
	}

	void queue_destroy(queue_t *q) {
		int err;
		qnode_t *curr, *next;

		mutex_lock(&mutex);

		curr = q->first;
		while (q->count > 0) {
			next = curr->next;
			free(curr);
			curr = next;
			q->count--;
		}
		err = pthread_cancel(q->qmonitor_tid);
		if (err) {
			printf("cannot cancel monitor: %s\n", strerror(err));
		}

		mutex_unlock(&mutex);

		free(q);
	}

	int queue_add(queue_t *q, int val) {
		mutex_lock(&mutex);

		q->add_attempts++;

		assert(q->count <= q->max_count);

		if (q->count == q->max_count) {
			mutex_unlock(&mutex);
			return 0;
		}

		qnode_t *new = malloc(sizeof(qnode_t));
		if (!new) {
			printf("Cannot allocate memory for new node\n");
			abort();
		}

		new->val = val;
		new->next = NULL;

		if (!q->first)
			q->first = q->last = new;
		else {
			q->last->next = new;
			q->last = q->last->next;
		}

		q->count++;
		q->add_count++;

		mutex_unlock(&mutex);

		return 1;
	}

	int queue_get(queue_t *q, int *val) {
		mutex_lock(&mutex);

		q->get_attempts++;

		assert(q->count >= 0);

		if (q->count == 0) {
			mutex_unlock(&mutex);
			return 0;
		}

		qnode_t *tmp = q->first;

		*val = tmp->val;
		q->first = q->first->next;

		free(tmp);
		q->count--;
		q->get_count++;

		mutex_unlock(&mutex);

		return 1;
	}

	void queue_print_stats(queue_t *q) {
		mutex_lock(&mutex);

		printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
			q->count,
			q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
			q->add_count, q->get_count, q->add_count -q->get_count);

		mutex_unlock(&mutex);
	}

#endif