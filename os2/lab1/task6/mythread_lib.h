typedef void *(*func)(void *);

typedef struct {
    func routine;
    void *arg;
    void *stack;
    int joined;
    int exited;
    void *res;
    pid_t tid;
} mythread_t;

int mythread_create(mythread_t *thread, func start_routine, void *arg);
int mythread_join(mythread_t *thread, void **res);
