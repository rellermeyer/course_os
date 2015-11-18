#include "tests.h"
#include "tests/test_priority_queue.h"
#include "data_structures/priority_queue.h"
#include "klibc.h"

#define NUM_TESTS 6
#define DEFAULT_COUNT 10
#define MIN_PRIORITY 20
#define MAX_PRIORITY -20

#define new(a) ((a*) kmalloc(sizeof(a)))
#define del(a) {if(a) kfree(a); a = 0;}
#define ret(q, e) {prq_free(q); return e;}

//This is where you define the tests you want to run. They return 1 on success and 0 on failure.

//Tests the create function
int test_prq_1() {
    prq_handle * queue;
    queue = prq_create();
    if (!queue) {
        ERROR("expected value");
        ret(queue, TEST_FAIL);
    }

    ret(queue, TEST_OK);

}

//Tests the enqueue and dequeue functions
int test_prq_2() {

    prq_handle * queue;
    prq_node * hn;
    int i;

    queue = prq_create();

    // Add reverse
    for (i = MIN_PRIORITY; i >= MAX_PRIORITY; i--) {
        hn = new(prq_node);
        hn->priority = i;
        prq_enqueue(queue, hn);
    }

    // Add forward
    for (i = MAX_PRIORITY; i <= MIN_PRIORITY; i++) {
        hn = new(prq_node);
        hn->priority = i;
        prq_enqueue(queue, hn);
    }

    for (i = MAX_PRIORITY; i <= MIN_PRIORITY; i++) {

        hn = prq_dequeue(queue);
        int priority_1 = hn->priority;
        del(hn);

        hn = prq_dequeue(queue);
        int priority_2 = hn->priority;
        del(hn);

        if (priority_1 != priority_2) {
            ERROR("[%d]: expected [%d]\n", priority_1, priority_2);
            ret(queue, TEST_FAIL);
        }
        if (priority_1 != i) {
            ERROR("[%d]: expected [%d]\n", priority_1, i);
            ret(queue, TEST_FAIL);
        }
        if (priority_2 != i) {
            ERROR("[%d]: expected [%d]\n", priority_2, i);
            ret(queue, TEST_FAIL);
        }
    }

    ret(queue, TEST_OK);
}

//Tests the enqueue and dequeue functions
int test_prq_3() {
    prq_handle * queue;
    prq_node * hn;
    int p;

    queue = prq_create();

    hn = new(prq_node);
    hn->priority = 0;
    prq_enqueue(queue, hn);

    hn = new(prq_node);
    hn->priority = -2;
    prq_enqueue(queue, hn);

    hn = new(prq_node);
    hn->priority = 1;
    prq_enqueue(queue, hn);

    hn = new(prq_node);
    hn->priority = -10;
    prq_enqueue(queue, hn);

    hn = new(prq_node);
    hn->priority = 30;
    prq_enqueue(queue, hn);

    hn = prq_dequeue(queue);
    p = hn->priority;
    del(hn);

    if (p != -10) {
        ERROR("[%d]: expected [%d]\n", p, -10);
        ret(queue, TEST_FAIL);
    }

    hn = prq_dequeue(queue);
    p = hn->priority;
    del(hn);

    if (p != -2) {
        ERROR("[%d]: expected [%d]\n", p, -2);
        ret(queue, TEST_FAIL);
    }

    hn = new(prq_node);
    hn->priority = 2;
    prq_enqueue(queue, hn);

    hn = prq_dequeue(queue);
    p = hn->priority;
    del(hn);

    if (p != 0) {
        ERROR("[%d]: expected [%d]\n", p, 0);
        ret(queue, TEST_FAIL);
    }

    prq_dequeue(queue);

    hn = prq_dequeue(queue);
    p = hn->priority;
    del(hn);

    if (p != 2) {
        ERROR("[%d]: expected [%d]\n", p, 2);
        ret(queue, TEST_FAIL);
    }

    hn = prq_dequeue(queue);
    p = hn->priority;
    del(hn);

    if (p != 30) {
        ERROR("[%d]: expected [%d]\n", p, 30);
        ret(queue, TEST_FAIL);
    }

    ret(queue, TEST_OK);

}

//testing peek function
int test_prq_4() {
    prq_handle * queue;
    prq_node * hn;
    queue = prq_create();

    if (prq_peek(queue) && queue->count < 1) {
        ERROR("expected [%d]\n", 0);
        ret(queue, TEST_FAIL);
    }

    hn = new(prq_node);
    hn->priority = 0;
    prq_enqueue(queue, hn);

    hn = new(prq_node);
    hn->priority = 1;
    prq_enqueue(queue, hn);

    hn = new(prq_node);
    hn->priority = 3;
    prq_enqueue(queue, hn);

    hn = prq_dequeue(queue);
    del(hn);

    hn = prq_dequeue(queue);
    del(hn);

    hn = prq_dequeue(queue);
    del(hn);

    if (prq_peek(queue) && queue->count < 1) {
        ERROR("expected [%d]\n", 0);
        ret(queue, TEST_FAIL);
    }

    ret(queue, TEST_OK);

}

//testing functionality of prq_create vs prq_fixed_create
int test_prq_5() {
    prq_handle * queue1;
    prq_node * hn1;
    queue1 = prq_create();

    hn1 = new(prq_node);
    hn1->priority = 0;
    prq_enqueue(queue1, hn1);

    prq_handle * queue2;
    prq_node * hn2;
    queue2 = prq_create_fixed(DEFAULT_COUNT);

    hn2 = new(prq_node);
    hn2->priority = 0;
    prq_enqueue(queue2, hn2);

    if (queue1->heap_size != queue2->heap_size) {
        ERROR("expected equal heap_size");
        ret(queue1, TEST_FAIL);
    }

    ret(queue1, TEST_OK);
}

// Test remove function
int test_prq_6() {
    prq_handle * queue1;
    prq_handle * queue2;
    prq_node * hn1;
    prq_node * hn2;
    prq_node * hn3;
    prq_node * hn4;
    prq_node * tmp1;
    prq_node * tmp2;

    queue1 = prq_create();
    queue2 = prq_create();

    hn1 = new(prq_node);
    hn1->priority = 1;

    hn2 = new(prq_node);
    hn2->priority = 2;

    hn3 = new(prq_node);
    hn3->priority = 3;

    hn4 = new(prq_node);
    hn4->priority = 4;

    prq_enqueue(queue1, hn1);
    prq_enqueue(queue1, hn2);
    prq_enqueue(queue1, hn3);
    prq_enqueue(queue1, hn4);
    prq_remove(queue1, hn1);
    prq_remove(queue1, hn3);
    // Double remove
    prq_remove(queue1, hn3);

    prq_enqueue(queue2, hn2);
    prq_enqueue(queue2, hn4);


    if (queue1->count != queue2->count) {
        ERROR("expected equal heap_size");
        for(int i = 0; i < queue1->count; ++i) {
            prq_node * tmp = prq_dequeue(queue1);
            del(tmp);
        }
        del(queue2);
        ret(queue1, TEST_FAIL);
    }

    for (int size = 0; size < queue1->count; ++size) {
        tmp1 = prq_dequeue(queue1);
        tmp2 = prq_dequeue(queue2);
        if (tmp1->priority != tmp2->priority) {
            del(tmp1);
            for(int i = 0; i < queue1->count; ++i) {
                prq_node * tmp = prq_dequeue(queue1);
                del(tmp);
            }
            del(queue2);
            ret(queue1, TEST_FAIL);
        }
        del(tmp1);
    }
    del(queue2);
    ret(queue1, TEST_OK);
}

//function running tests
void run_prq_tests() {
    Test *tests[NUM_TESTS];
    tests[0] = create_test("test_prq_1", &test_prq_1);
    tests[1] = create_test("test_prq_2", &test_prq_2);
    tests[2] = create_test("test_prq_3", &test_prq_3);
    tests[3] = create_test("test_prq_4", &test_prq_4);
    tests[4] = create_test("test_prq_5", &test_prq_5);
    tests[5] = create_test("test_prq_6", &test_prq_6);

    run_tests(tests, NUM_TESTS);
}

