#include "include/testing_suite.h"
#include "klibc.h"
#include "mem_alloc.h"

TEST_SUITE create_test_suite() {
    TEST_SUITE tests;
    tests.TESTS = (TEST*) malloc(sizeof(TEST));
    tests.NUM_OF_TESTS = 0;//malloc(sizeof(TEST_SUITE));
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

    os_printf("[==========] Running test suite\n");
    os_printf("[----------] Running %d tests...\n", tests.NUM_OF_TESTS);
    os_printf("[----------]\n");

    for (i = 0; i < tests.NUM_OF_TESTS; ++i) {
    	os_printf("[RUN       ] %s\n", (tests.TESTS[i]).test_name);
        if ((tests.TESTS[i]).ptr_to_test() == 1) {
        	os_printf("[        OK] %s\n", (tests.TESTS[i]).test_name);
        }
        else {
        	os_printf("[      FAIL] %s\n", (tests.TESTS[i]).test_name);
        }
    }

    os_printf("[==========] Done\n");
}
