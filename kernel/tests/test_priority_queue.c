#include "../include/tests.h"
#include "../include/tests/test_priority_queue.h"

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
	init_queue(&queue, n);

	for (i = 0; i < n; ++i) {
		hn.value = 50 % 10000;
		printf("prq_enqueue node with value: %d\n", hn.value);
		prq_enqueue(hn, &queue);
	}

	printf("\nprq_dequeue all values:\n");

	for (i = 0; i < n; ++i) {
		hn = prq_dequeue(&queue);
		printf(
				"prq_dequeued node with value: %d, queue size after removal: %d\n",
				hn.value, queue.size);
	}

	return 1;
}

void init_prq_tests() {
	printf("STARTING\n");

	TEST test_1 = create_test("test_prq_1", &test_prq_1);
	TEST test_2 = create_test("test_prq_2", &test_prq_2);

	printf("CREATED TESTS\n");
	TEST_SUITE tests = create_test_suite();
	printf("CREATED SUITE\n");

	add_test(test_1, &tests);
	add_test(test_2, &tests);

	printf("ADDED TESTS\n");
	run_tests(tests);
	printf("DONE\n");
}

