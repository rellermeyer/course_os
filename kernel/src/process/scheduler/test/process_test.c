#include <test.h>
#include <stdio.h>
#include <thread.h>

#include "../include/process.h"

TEST_CREATE(test_process_smoke, {
    Process *process = create_process((void *) 63, NULL);

    ASSERT_EQ(process->priority, DEFAULT_PROCESS_PRIORITY);
    ASSERT_NOT_NULL(process->vas);
    ASSERT_NOT_NULL(vpa_get(process->threads, 0));
    ASSERT_EQ(vpa_get(process->threads, 1), NULL);

    Thread *thread = vpa_get(process->threads, 0);
    ASSERT_EQ(thread->registers.PC, 63);
    ASSERT_EQ(thread->process, process);

    free_process(process);
});
