#ifndef _TESTS_H_
#define _TESTS_H_

#include "global_defs.h"

// Each test consists of a descriptor and function pointer to the test function.
typedef struct Test {
  char *test_name;
  int (*testptr)();
} Test;

// Run_tests takes an array of tests which it runs and prints the results.
void run_tests(Test *test_group[], int num_tests);
void run_fs_tests();

int test1();
int test2();

Test* create_test(char *name, int (*test_function)());

#endif
