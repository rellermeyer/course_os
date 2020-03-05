#include <test.h>
#include <stdio.h>

#include "../include/scheduler.h"

TEST_CREATE(test_scheduler_smoke, {
    Scheduler *scheduler = create_scheduler();

    free_scheduler(scheduler);
});
