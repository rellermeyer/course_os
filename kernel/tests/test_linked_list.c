#include "../include/tests.h"
#include "../include/tests/test_linked_list.h"
#include "../include/linked_list.h"
#include "klibc.h"

#define NUM_TESTS 6

#define new(a) ((a*) kmalloc(sizeof(a)))

//This is where you define the tests you want to run. They return 1 on success and 0 on failure.

//Tests the create functions
int test_llist_1() {
	llist_handle* llist;
	llist_node* llist_node;

	llist = llist_create();
	if (!llist) {
		os_printf("expected value");
		llist_free(llist);
		return TEST_FAIL;
	}

	void* data = new(int);
	llist_node = llist_create_node(data);

	llist_free(llist);
	llist_free_node(llist_node);
	return TEST_OK;
}

// Test enqueue and dequeue functions
int test_llist_2() {

	llist_handle* llist;
	llist = llist_create();

	int* data1;
	int* data2;
	int* data3;

	data1 = new(int);
	*data1 = 0;
	llist_enqueue(llist, data1);

	data2 = new(int);
	*data2 = 1;
	llist_enqueue(llist, data2);

	data3 = new(int);
	*data3 = 2;
	llist_enqueue(llist, data3);

	if (llist_dequeue(llist) != data3 || llist_dequeue(llist) != data2
			|| llist_dequeue(llist) != data1) {
		kfree(data1);
		kfree(data2);
		kfree(data3);
		llist_free(llist);
		return TEST_FAIL;
	}

	kfree(data1);
	kfree(data2);
	kfree(data3);
	llist_free(llist);
	return TEST_OK;
}

// Test insert and remove_at function
int test_llist_3() {
	llist_handle* llist;
	llist = llist_create();

	int* data1;
	int* data2;
	int* data3;

	data1 = new(int);
	*data1 = 0;
	llist_insert(llist, data1, 0);

	data2 = new(int);
	*data2 = 1;
	llist_insert(llist, data2, 1);

	data3 = new(int);
	*data3 = 2;
	llist_insert(llist, data3, 2);

	llist_remove_at(llist, 0);
	if (llist_count(llist) != 2) {
		kfree(data1);
		kfree(data2);
		kfree(data3);
		llist_free(llist);
		return TEST_FAIL;
	}
	llist_remove_at(llist, 0);
	if (llist_count(llist) != 1) {
		kfree(data1);
		kfree(data2);
		kfree(data3);
		llist_free(llist);
		return TEST_FAIL;
	}
	llist_remove_at(llist, 0);

	if (llist_count(llist) != 0) {
		kfree(data1);
		kfree(data2);
		kfree(data3);
		llist_free(llist);
		return TEST_FAIL;
	}

	kfree(data1);
	kfree(data2);
	kfree(data3);
	llist_free(llist);
	return TEST_OK;
}

// Test insert and remove_at function
int test_llist_4() {
	llist_handle* llist;
	llist = llist_create();

	int* data1;
	int* data2;
	int* data3;
	int* data4;

	data1 = new(int);
	*data1 = 0;
	llist_insert(llist, data1, 0);

	data2 = new(int);
	*data2 = 1;
	llist_insert(llist, data2, 1);

	data3 = new(int);
	*data3 = 2;
	llist_insert(llist, data3, 2);

	data4 = new(int);
	*data4 = 3;
	llist_enqueue(llist, data4);

	int* tmp1 = llist_get_by_index(llist, 0);
	int* tmp2 = llist_get_by_index(llist, 1);
	int* tmp3 = llist_get_by_index(llist, 2);
	int* tmp4 = llist_get_by_index(llist, 3);

	if (tmp1 != data1 || tmp2 != data2 || tmp3 != data3 || tmp4 != data4
			|| llist_count(llist) != 4) {
		kfree(tmp1);
		kfree(tmp2);
		kfree(tmp3);
		llist_free(llist);
		return TEST_FAIL;
	}

	kfree(tmp1);
	kfree(tmp2);
	kfree(tmp3);
	llist_free(llist);
	return TEST_OK;
}

// Test set_data and get_node functions
int test_llist_5() {
	llist_handle* llist;
	llist_node* llist_node1;
	llist_node* llist_node2;
	llist_node* llist_node3;
	llist_node* llist_node4;

	int* data1;
	int* data2;
	int* data3;
	int* data4;

	llist = llist_create();

	data1 = new(int);
	*data1 = 0;
	llist_node1 = llist_create_node(data1);
	llist_enqueue(llist, llist_node1);

	data2 = new(int);
	*data2 = 1;
	llist_node2 = llist_create_node(data2);
	llist_enqueue(llist, llist_node2);

	data3 = new(int);
	*data3 = 2;
	llist_node3 = llist_create_node(data3);
	llist_enqueue(llist, llist_node3);

	data4 = new(int);
	*data4 = 3;
	llist_node4 = llist_create_node(data4);
	llist_enqueue(llist, llist_node4);

	if (llist_get_node(llist, 0) != llist_node1
			|| llist_get_node(llist, 1) != llist_node2
			|| llist_get_node(llist, 2) != llist_node3
			|| llist_get_node(llist, 3) != llist_node4) {

		kfree(data1);
		kfree(data2);
		kfree(data3);
		kfree(data4);
		llist_free(llist);
		return TEST_FAIL;
	}

	llist_set_data(llist_node1, data4);
	llist_set_data(llist_node2, data3);
	llist_set_data(llist_node3, data2);
	llist_set_data(llist_node4, data1);

	if (llist_node1->data != data4 || llist_node2->data != data3
			|| llist_node3->data != data2 || llist_node4->data != data1) {
		kfree(data1);
		kfree(data2);
		kfree(data3);
		kfree(data4);
		llist_free(llist);
		return TEST_FAIL;
	}

	kfree(data1);
	kfree(data2);
	kfree(data3);
	kfree(data4);
	llist_free(llist);
	return TEST_OK;
}

// Test count function
int test_llist_6() {
	llist_handle* llist;
	llist = llist_create();

	int* data1;
	int* data2;
	int* data3;
	int* data4;

	data1 = new(int);
	*data1 = 0;
	llist_enqueue(llist, data1);

	if (llist_count(llist) != 1) {
		kfree(data1);
		llist_free(llist);
		return TEST_FAIL;
	}

	data2 = new(int);
	*data2 = 1;
	llist_enqueue(llist, data2);

	if (llist_count(llist) != 2) {
		kfree(data1);
		kfree(data2);
		llist_free(llist);
		return TEST_FAIL;
	}

	data3 = new(int);
	*data3 = 2;
	llist_enqueue(llist, data3);

	if (llist_count(llist) != 3) {
		kfree(data1);
		kfree(data2);
		kfree(data3);
		llist_free(llist);
		return TEST_FAIL;
	}

	kfree(llist_dequeue(llist));

	if (llist_count(llist) != 2) {
		kfree(data1);
		kfree(data2);
		llist_free(llist);
		return TEST_FAIL;
	}

	data4 = new(int);
	*data4 = 3;
	llist_enqueue(llist, data4);

	if (llist_count(llist) != 3) {
		kfree(data1);
		kfree(data2);
		kfree(data4);
		llist_free(llist);
		return TEST_FAIL;
	}

	kfree(data1);
	kfree(data2);
	kfree(data4);
	llist_free(llist);
	return TEST_OK;
}

//function running tests
void run_llist_tests() {
	Test *tests[NUM_TESTS];
	tests[0] = create_test("test_llist_1", &test_llist_1);
	tests[1] = create_test("test_llist_2", &test_llist_2);
	tests[2] = create_test("test_llist_3", &test_llist_3);
	tests[3] = create_test("test_llist_4", &test_llist_4);
	tests[4] = create_test("test_llist_5", &test_llist_5);
	tests[5] = create_test("test_llist_6", &test_llist_6);

	run_tests(tests, NUM_TESTS);
}

