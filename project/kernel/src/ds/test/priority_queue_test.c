#include <priority_queue.h>
#include <stdlib.h>
#include <test.h>

TEST_CREATE(test_prq_1, {
    prq_handle * const queue = prq_create();
    ASSERT_NOT_NULL(queue);
    prq_free(queue);
})

TEST_CREATE(test_prq_2, {
    static const int MIN_PRIORITY = 20;
    static const int MAX_PRIORITY = -20;

    prq_handle * const queue = prq_create();

    // Add reverse
    for (int i = MIN_PRIORITY; i >= MAX_PRIORITY; i--) {
        prq_node * const hn = prq_create_node();
        hn->priority = i;
        prq_enqueue(queue, hn);
    }

    // Add forward
    for (int i = MAX_PRIORITY; i <= MIN_PRIORITY; i++) {
        prq_node * const hn = prq_create_node();
        hn->priority = i;
        prq_enqueue(queue, hn);
    }

    for (int i = MAX_PRIORITY; i <= MIN_PRIORITY; i++) {
        prq_node * const hn_1 = prq_dequeue(queue);
        prq_node * const hn_2 = prq_dequeue(queue);

        ASSERT_EQ(hn_1->priority, i);
        ASSERT_EQ(hn_2->priority, i);

        prq_free_node(hn_1);
        prq_free_node(hn_2);
    }

    prq_free(queue);
    PASS();
})

// Tests the enqueue and dequeue functions
TEST_CREATE(test_prq_3, {
    prq_handle * const queue = prq_create();
    prq_node * hn;

    hn = prq_create_node();
    hn->priority = 0;
    prq_enqueue(queue, hn);

    hn = prq_create_node();
    hn->priority = -2;
    prq_enqueue(queue, hn);

    hn = prq_create_node();
    hn->priority = 1;
    prq_enqueue(queue, hn);

    hn = prq_create_node();
    hn->priority = -10;
    prq_enqueue(queue, hn);

    hn = prq_create_node();
    hn->priority = 30;
    prq_enqueue(queue, hn);

    hn = prq_dequeue(queue);
    ASSERT_EQ(hn->priority, -10);
    prq_free_node(hn);

    hn = prq_dequeue(queue);
    ASSERT_EQ(hn->priority, -2);
    prq_free_node(hn);

    hn = prq_create_node();
    hn->priority = 2;
    prq_enqueue(queue, hn);

    hn = prq_dequeue(queue);
    ASSERT_EQ(hn->priority, 0);
    prq_free_node(hn);

    hn = prq_dequeue(queue);
    prq_free_node(hn);

    hn = prq_dequeue(queue);
    ASSERT_EQ(hn->priority, 2);
    prq_free_node(hn);

    hn = prq_dequeue(queue);
    ASSERT_EQ(hn->priority, 30);
    prq_free_node(hn);

    prq_free(queue);
    PASS();
})

// Testing peek function
TEST_CREATE(test_prq_4, {
    prq_handle * const queue = prq_create();
    prq_node * hn;

    ASSERT_NULL(prq_peek(queue));
    ASSERT_EQ(prq_count(queue), 0);

    hn = prq_create_node();
    hn->priority = 0;
    prq_enqueue(queue, hn);

    hn = prq_create_node();
    hn->priority = 1;
    prq_enqueue(queue, hn);

    hn = prq_create_node();
    hn->priority = 3;
    prq_enqueue(queue, hn);

    hn = prq_dequeue(queue);
    prq_free_node(hn);

    hn = prq_dequeue(queue);
    prq_free_node(hn);

    hn = prq_dequeue(queue);
    prq_free_node(hn);

    ASSERT_NULL(prq_peek(queue));
    ASSERT_EQ(prq_count(queue), 0);

    prq_free(queue);
    PASS();
})

// Test remove function
TEST_CREATE(test_prq_5, {
    prq_handle * const queue_1 = prq_create();
    prq_handle * const queue_2 = prq_create();

    prq_node * hn_1 = prq_create_node();
    hn_1->priority = 1;

    prq_node * hn_2 = prq_create_node();
    hn_2->priority = 2;

    prq_node * hn_3 = prq_create_node();
    hn_3->priority = 3;

    prq_node * hn_4 = prq_create_node();
    hn_4->priority = 4;

    prq_enqueue(queue_1, hn_1);
    prq_enqueue(queue_1, hn_2);
    prq_enqueue(queue_1, hn_3);
    prq_enqueue(queue_1, hn_4);
    prq_remove(queue_1, hn_2);
    prq_remove(queue_1, hn_4);
    // Double remove
    prq_remove(queue_1, hn_4);
    prq_enqueue(queue_2, hn_2);
    prq_enqueue(queue_2, hn_4);

    ASSERT_EQ(prq_count(queue_1), 2);
    ASSERT_EQ(prq_count(queue_2), 2);

    prq_node * dequeued_hn;

    dequeued_hn = prq_dequeue(queue_1);
    ASSERT_EQ(dequeued_hn->priority, 1);
    prq_free_node(dequeued_hn);

    dequeued_hn = prq_dequeue(queue_1);
    ASSERT_EQ(dequeued_hn->priority, 3);
    prq_free_node(dequeued_hn);

    dequeued_hn = prq_dequeue(queue_2);
    ASSERT_EQ(dequeued_hn->priority, 2);
    prq_free_node(dequeued_hn);

    dequeued_hn = prq_dequeue(queue_2);
    ASSERT_EQ(dequeued_hn->priority, 4);
    prq_free_node(dequeued_hn);

    prq_free(queue_1);
    prq_free(queue_2);
    PASS();
})
