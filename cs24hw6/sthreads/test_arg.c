#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sthread.h"

// prints out an integer argument
static void t1(void * n) {
		int a = *((int *) n);
		printf("In thread 1: %d\n", a);
		sthread_yield();
}

// prints out an integer argument
static void t2(void * n) {
		int a = *((int *) n);
		printf("In thread 2: %d\n", a);
		sthread_yield();
	}

int main(int argc, char ** argv) {

	// arbitrary integer argument
	int num = 0;

	// create threads
	sthread_create(t1, (void *) &num);
	sthread_create(t2, (void *) &num);
	// start threads
	sthread_start();
	return 0;
}