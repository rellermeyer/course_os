#ifndef TEST_H
#define TEST_H

#include <stdint.h>
// Each test consists of its name and a 
// function pointer that is to be tested
typedef struct Test {
	char* test_name;
	uint32_t (*testptr)();
} Test;

Test* create_test(char *name, uint32_t (*function_to_test)(void));


// Run_tests takes an array of tests which it runs and prints the results.
void run_tests(Test *test_group[], int num_tests);

uint32_t test1();
uint32_t test2();
#endif
