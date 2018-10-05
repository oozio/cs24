#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sthread.h"

// counts up to integer argument
static void t(void * times) {
	// cast argument to integer
	int a = *((int *) times);
	for (int i = 0; i < a; i++) {
		printf("In a thread: %d/%d\n", i, a);
		sthread_yield();
	}
}

int main(int argc, char ** argv) {

	// arbitrary integer arguments
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 100;
	int e = 5;

	// create threads
	sthread_create(t, (void *) &a);
	sthread_create(t, (void *) &b);
	sthread_create(t, (void *) &c);
	sthread_create(t, (void *) &d);
	sthread_create(t, (void *) &e);

	// start threads
	sthread_start();
	return 0;
}