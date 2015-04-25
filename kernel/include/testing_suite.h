//in process: integrate testing_suite into tests
#ifndef TESTING_SUITE_H
#define TESTING_SUITE_H

typedef struct TEST {
	char* test_name;
	int   (*ptr_to_test)();
} TEST;

TEST create_test(char *name, int (*function_to_test)());

typedef struct TEST_SUITE {
    TEST* TESTS;
    int NUM_OF_TESTS;
} TEST_SUITE;

TEST_SUITE create_test_suite();

void add_test(TEST test, TEST_SUITE *tests);

void run_tests(TEST_SUITE tests);

#endif
*/