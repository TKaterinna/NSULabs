#include <ucontext.h>

typedef void *(*func)(void *);

typedef struct {
    func routine;
    void *arg;
    ucontext_t context;
} uthread_t;

int uthread_create(uthread_t **thread, void *(start_routine), void *arg);
void schedule(void);
void setmain(uthread_t main_thread);