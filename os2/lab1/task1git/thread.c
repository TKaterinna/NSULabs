#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int global = 10; //.data
//.bss if "int global;"

void *mythread(void *arg) {
	int local = 1; //stack
	static int static_local = 2; //.data
	const int const_local = 3; //stack

	printf("local = %d, global = %d\n", local, global);


	printf("mythread [%d %d %d %ld]: Hello from mythread!\n", getpid(), getppid(), gettid(), pthread_self());
	printf("addr: local = %p, static_local = %p, const_local = %p, global = %p\n", &local, &static_local, &const_local, &global);

	local += 1;
	global +=1;
	printf("new local = %d, global = %d\n", local, global);

	sleep(1000);
	
	return NULL;
}

int main() {
	int thread_count = 5;
	pthread_t tid[thread_count];
	int err;


	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

	// sleep(15);

	for (int i = 0; i < thread_count; i++) {
		printf("main start create\n");
		err = pthread_create(&tid[i], NULL, mythread, NULL);
		printf("main stop create\n");
		if (err) {
	    	printf("main: pthread_create() failed: %s\n", strerror(err));
			return -1;
		}
		printf("tid %ld\n", tid[i]);
		
		sleep(1);
	}

	sleep(1000);

	for (int i = 0; i < thread_count; i++) {
		err = pthread_join(tid[i], NULL);
		if (err) {
	    	printf("main: pthread_join() failed: %s\n", strerror(err));
			return -1;
		}
	}

	return 0;
}

