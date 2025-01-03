all:
	gcc -c uthread_lib.c -o uthread_lib.o -Wall
	ar rcs libuthread.a uthread_lib.o
	gcc task7.c -L. -luthread -o task7.out -Wall