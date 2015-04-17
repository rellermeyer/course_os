#include "../include/tests.h"
#include "../include/tests/test_priority_queue.h"
#include "../include/priority_queue.h"
#include "klibc.h"

#define NUMTESTS 2

//This is where you define the tests you want to run. They return 1 on success and 0 on failure.
int test_prq_1() {
	return 1;
}

int test_prq_2() {
	int n;
	int i;
	prq_handle queue;
	prq_node hn;
	n = 50;
	prq_init_queue(&queue, n);

	for (i = 0; i < n; ++i) {
		hn.value = 50 % 10000;
		os_printf("prq_enqueue node with value: %d\n", hn.value);
		prq_enqueue(hn, &queue);
	}

	os_printf("\nprq_dequeue all values:\n");

	for (i = 0; i < n; ++i) {
		hn = prq_dequeue(&queue);
		os_printf(
				"prq_dequeued node with value: %d, queue size after removal: %d\n",
				hn.value, queue.size);
	}

	return 1;
}

void run_prq_tests() {
	//This is the array of test functions that we pass to the test runner.
	Test *tests[NUMTESTS];

	//Tests are created by providing a descriptor that will be printed out when the test is run, and the address of the test function.
	tests[0] = create_test("test_prq_1", &test_prq_1);

	tests[1] = create_test("test_prq_2", &test_prq_2);

	run_tests(tests, NUMTESTS);
}

