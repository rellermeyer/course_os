//in process: integrate testing_suite into tests
#include "include/testing_suite.h"
#include <stdio.h>
#include <stdlib.h>

TEST create_test(char* name, int (*function_to_test)()) {
    TEST test = {name, function_to_test};

    return test;
}

TEST_SUITE create_test_suite() {
    TEST_SUITE tests = {malloc(sizeof(TEST)), 0};//malloc(sizeof(TEST_SUITE));
    return tests;
}

void add_test(TEST test, TEST_SUITE *tests) {
    *(tests->TESTS) = test;
    ++tests->TESTS;
    ++tests->NUM_OF_TESTS;
}

void run_tests(TEST_SUITE tests) {
    int i;
    tests.TESTS -= tests.NUM_OF_TESTS;

    printf("[==========] Running test suite\n");
    printf("[----------] Running %d tests...\n", tests.NUM_OF_TESTS);
    printf("[----------]\n");

    for (i = 0; i < tests.NUM_OF_TESTS; ++i) {
        printf("[RUN       ] %s\n", (tests.TESTS[i]).test_name);
        if ((tests.TESTS[i]).ptr_to_test() == 1) {
	    printf("[        OK] %s\n", (tests.TESTS[i]).test_name);
        }
        else {
	    printf("[      FAIL] %s\n", (tests.TESTS[i]).test_name);
        }
    }

    printf("[==========] Done\n");
}
