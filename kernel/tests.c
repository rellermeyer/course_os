#include "global_defs.h"
#include "tests.h"
#include "klibc.h"
#include "mem_alloc.h"

//This function executes and displays results of test set given to it.
void run_tests(Test *tests[], int num_tests) {
	int i;
	for (i = 0; i < num_tests; i++) {
		INFO("-------------------------------\n"
				"%s:\n"
				"-------------------------------\n\n", tests[i]->test_name);

		//evaluates test here, if it passes prints PASSES else FAILS
		int res = tests[i]->testptr;

		INFO("\n-------------------------------\n"
				"%s (%s)\n"
				"-------------------------------\n",
				tests[i]->testptr() ? "FAILS" : "PASSES", tests[i]->test_name);
	}
}

Test* create_test(char *name, int (*test_function)(void*)) {
	Test *test = (Test*) kmalloc(sizeof(Test));
	test->test_name = name;
	test->testptr = test_function;
	return test;
}

int test1() {
	return TRUE;
}

int test2() {
	return FALSE;
}
