#include "../include/tests.h"
#include "../include/tests/test_priority_queue.h"
#include "../include/priority_queue.h"
#include "klibc.h"

// cd ~/Desktop/workspace/course_os
// git add .
// git commit -a -m 'message goes here'
// git push

#define NUM_TESTS 2
#define MIN_PRIORITY 20
#define MAX_PRIORITY -20

#define new(a) ((a*) kmalloc(sizeof(a)))
#define del(a) {if(a) kfree(a); a = 0;}
#define ret(q, e) {prq_free(q); return e;}

//This is where you define the tests you want to run. They return 1 on success and 0 on failure.
int test_prq_1() {

    prq_handle * queue;
    prq_node * hn;
    int i;

    queue = prq_create(100);

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
            os_printf("[%d]: expected [%d]\n", priority_1,
                    priority_2);
            ret(queue, TEST_FAIL);
        }
        if (priority_1 != i) {
            os_printf("[%d]: expected [%d]\n", priority_1, i);
            ret(queue, TEST_FAIL);
        }
        if (priority_2 != i) {
            os_printf("[%d]: expected [%d]\n", priority_2, i);
            ret(queue, TEST_FAIL);
        }
    }

    ret(queue, TEST_OK);
}

int test_prq_2() {
    prq_handle * queue;
    prq_node * hn;
    int p;

    queue = prq_create(10);

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
        os_printf("[%d]: expected [%d]\n", p, -10);
        ret(queue, TEST_FAIL);
    }

    hn = prq_dequeue(queue);
    p = hn->priority;
    del(hn);

    if (p != -2) {
        os_printf("[%d]: expected [%d]\n", p, -2);
        ret(queue, TEST_FAIL);
    }

    hn = new(prq_node);
    hn->priority = 2;
    prq_enqueue(queue, hn);

    hn = prq_dequeue(queue);
    p = hn->priority;
    del(hn);

    if (p != 0) {
        os_printf("[%d]: expected [%d]\n", p, 0);
        ret(queue, TEST_FAIL);
    }

    prq_dequeue(queue);

    hn = prq_dequeue(queue);
    p = hn->priority;
    del(hn);

    if (p != 2) {
        os_printf("[%d]: expected [%d]\n", p, 2);
        ret(queue, TEST_FAIL);
    }

    hn = prq_dequeue(queue);
    p = hn->priority;
    del(hn);

    if (p != 30) {
        os_printf("[%d]: expected [%d]\n", p, 30);
        ret(queue, TEST_FAIL);
    }

    ret(queue, TEST_OK);

}

void run_prq_tests() {
    Test *tests[NUM_TESTS];
    tests[0] = create_test("test_prq_1", &test_prq_1);
    tests[1] = create_test("test_prq_2", &test_prq_2);
    run_tests(tests, NUM_TESTS);
}

