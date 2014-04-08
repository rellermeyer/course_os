// Each test consists of a descriptor and function pointer to the test function.
typedef struct Test {
  char *test_name;
  int (*testptr)();
} Test;

// Run_tests takes an array of tests which it runs and prints the results.
void run_tests(Test *test_group[], int num_tests);

Test* create_test(char *name, int (*test_function)());
