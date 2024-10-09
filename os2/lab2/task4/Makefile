all:
	gcc -c locks.c -o locks.o -Wall
	ar rcs liblocks.a locks.o
	gcc -c queue.c -o queue.o -Wall
	ar rcs libqueue.a queue.o
	gcc task4.c -L. -lqueue -llocks -o task4.out -Wall