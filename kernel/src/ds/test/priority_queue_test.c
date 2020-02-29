#include <test.h>
#include <priority_queue.h>
#include <stdlib.h>

#define DEFAULT_COUNT 10
#define MIN_PRIORITY 20
#define MAX_PRIORITY -20

TEST_CREATE(test_pq_1, {
    prq_handle *queue = prq_create();
    ASSERT_NOT_NULL(queue);


    prq_free(queue);
})

// TODO: Port all tests
