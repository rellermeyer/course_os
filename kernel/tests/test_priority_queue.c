#include "../include/tests.h"
#include "../include/tests/test_priority_queue.h"
#include "../include/priority_queue.h"
#include "klibc.h"

#define NUM_TESTS 2
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
	prq_handle queue;
	prq_node hn;
	int i;

	prq_init(&queue, 10);

	hn.priority = 0;
	prq_enqueue(hn, &queue);
	hn.priority = -2;
	prq_enqueue(hn, &queue);
	hn.priority = 1;
	prq_enqueue(hn, &queue);
	hn.priority = -10;
	prq_enqueue(hn, &queue);
	hn.priority = 30;
	prq_enqueue(hn, &queue);

	if((i = prq_dequeue(&queue).priority) != -10){
		os_printf("test_prq_1 [%d]: expected [%d]\n", i, -10);
		return TEST_FAIL;
	}

	if((i = prq_dequeue(&queue).priority) != -2){
		os_printf("test_prq_1 [%d]: expected [%d]\n", i, -2);
		return TEST_FAIL;
	}

	hn.priority = 2;
	prq_enqueue(hn, &queue);

	if((i = prq_dequeue(&queue).priority) != 0){
		os_printf("test_prq_1 [%d]: expected [%d]\n", i, 0);
		return TEST_FAIL;
	}

	prq_dequeue(&queue);

	if((i = prq_dequeue(&queue).priority) != 2){
		os_printf("test_prq_1 [%d]: expected [%d]\n", i, 2);
		return TEST_FAIL;
	}

	if((i = prq_dequeue(&queue).priority) != 30){
		os_printf("test_prq_1 [%d]: expected [%d]\n", i, 30);
		return TEST_FAIL;
	}

	return TEST_OK;

}

void run_prq_tests() {
	Test *tests[NUM_TESTS];
	tests[0] = create_test("test_prq_1", &test_prq_1);
	tests[1] = create_test("test_prq_2", &test_prq_2);
	run_tests(tests, NUM_TESTS);
}

