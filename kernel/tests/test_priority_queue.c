#include "../include/tests.h"
#include "../include/tests/test_priority_queue.h"
#include "../include/priority_queue.h"
#include "klibc.h"

#define NUMTESTS 2
#define MIN_PRIORITY 20
#define MAX_PRIORITY -20

//This is where you define the tests you want to run. They return 1 on success and 0 on failure.
int test_prq_1() {
	prq_handle queue;
	prq_node hn;
	int i;

	prq_init(&queue, 100);

	// Add reverse
	for (i = MIN_PRIORITY; i >= MAX_PRIORITY; i--) {
		hn.priority = i;
		prq_enqueue(hn, &queue);
	}

	// Add forward
	for (i = MAX_PRIORITY; i <= MIN_PRIORITY; i++) {
		hn.priority = i;
		prq_enqueue(hn, &queue);
	}

	for (i = MAX_PRIORITY; i <= MIN_PRIORITY; i++) {
		hn = prq_dequeue(&queue);
		int priority_1 = hn.priority;
		hn = prq_dequeue(&queue);
		int priority_2 = hn.priority;
		if (priority_1 != priority_2) {
			os_printf("test_prq_1 [%d]: expected [%d]\n", priority_1,
					priority_2);
			return TEST_FAIL;
		}
		if (priority_1 != i) {
			os_printf("test_prq_1 [%d]: expected [%d]\n", priority_1, i);
			return TEST_FAIL;
		}
		if (priority_2 != i) {
			os_printf("test_prq_1 [%d]: expected [%d]\n", priority_2, i);
			return TEST_FAIL;
		}
	}

	return TEST_OK;
}

int test_prq_2() {
	return TEST_OK;
}

void run_prq_tests() {
	//This is the array of test functions that we pass to the test runner.
	Test *tests[NUMTESTS];

	//Tests are created by providing a descriptor that will be printed out when the test is run, and the address of the test function.
	tests[0] = create_test("test_prq_1", &test_prq_1);

	tests[1] = create_test("test_prq_2", &test_prq_2);

	run_tests(tests, NUMTESTS);
}

