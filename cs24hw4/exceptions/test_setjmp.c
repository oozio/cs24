#include "my_setjmp.h"
#include <stdlib.h>
#include <stdio.h>

//double-check at runtime whether we're using default
// setjmp and longjmp or custom setjmp and longjmp
#ifdef ENABLE_MY_SETJMP
	int def = 1;
#else
	int def = 0;
#endif

int PASS = 0;

//import functions
extern void my_longjmp(jmp_buf buf, int r);
extern int my_setjmp(jmp_buf buf);

// tests whether the return values of setjmp and longjmp
//  are as expected
void test_return_value(int n) {
	jmp_buf env;
	PASS = 0;
	int n1 = setjmp(env);
	if (n1 == 0){
		longjmp(env,n);
	}	
	if (n1 == n) {
		PASS = 1;
	}

	if (PASS == 1){
		printf("setjmp returns 0: PASS\n");
		printf("longjmp sets setjmp's return to n: PASS\n");
		printf("longjmp sets setjmp's return to 1 if n = 0: PASS\n");
	} else {
		printf("return value tests: FAIL\n");
	}

}


//helper function used in testing whether set/longjmp works 
//  over multiple functions
void f1(jmp_buf env, int *n) {
	*n = setjmp(env);
}

//helper function used in testing whether set/longjmp works 
//  over multiple functions
void f2(jmp_buf env, int *n) {
	longjmp(env, *n);
}
 
//tests whether set/longjmp works over multiple functions
void two_functions() {
	PASS = 0;
	jmp_buf env;
	f1(env, &PASS);
	f2(env, &PASS);
	printf("works across two functions: ");
	if (PASS == 1){
		printf("PASS\n");
	} else {
		printf("FAIL\n");
	}
}


//tests whether changes are limited to buffer
void buffer() {
	int a = 1;
	jmp_buf env;
	int b = 2;
	int n = setjmp(env);
	if (n == 0) {
		longjmp(env,0);
	}
	// checks that a and b are unchanged
	printf("changes limited to buffer: ");
	if (a == 1 && b == 2) {
		printf("PASS\n");
	} else {
		printf("FAIL\n");
	}
}


//tests whether local variables can be recovered
void local() {
	volatile int a = 1;
	jmp_buf env;
	int n = setjmp(env);
	
	// since a is volatile, it should take on the value
	//   given between setjmp and longjmp
	if (n == 0) {
		a = 2;
		longjmp(env, 3);
	}

	printf("correct interaction with changed local variables: ");
	if (a == 2) {
		printf("PASS\n");
	} else {
		printf("FAIL\n");
	}
}

int main() {
	test_return_value(5);
	two_functions();
	buffer();
	local();
}