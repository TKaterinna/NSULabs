all:
	gcc -c mythread_lib.c -o mythread_lib.o
	ar rcs libmythread.a mythread_lib.o
	gcc task6.c -L. -lmythread -o task6.out