#include <test.h>
#include <stdio.h>

#include <thread.h>

TEST_CREATE(test_thread_smoke, {
    Thread *thread = create_thread((void *) 42, NULL);

    ASSERT_EQ(thread->state, Runnable);
    ASSERT_EQ(thread->registers.PC, 42);
    ASSERT_NEQ(thread->registers.SP, 0);

    free_thread(thread);
});
