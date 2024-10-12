all:
	gcc -c locks.c -o locks.o
	ar rcs liblocks.a locks.o
	gcc -c queue.c -o queue.o
	ar rcs libqueue.a queue.o
	gcc task4.c -L. -lqueue -llocks -lpthread -o task4.out