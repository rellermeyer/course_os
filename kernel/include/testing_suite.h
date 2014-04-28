#ifndef TESTING_SUITE_H
#define TESTING_SUITE_H

#include "test.h"

typedef struct TEST_SUITE {
    TEST* TESTS;
    int NUM_OF_TESTS;
} TEST_SUITE;

TEST_SUITE create_test_suite();

void add_test(TEST test, TEST_SUITE *tests);

void run_tests(TEST_SUITE tests);

#endif
