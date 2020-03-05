#include <stdlib.h>
#include <stdio.h>

#include "./include/scheduler.h"

Scheduler *create_scheduler() {
    Scheduler *scheduler = kmalloc(sizeof(Scheduler));
    scheduler->current_thread = NULL;

    return scheduler;
}

void free_scheduler(Scheduler *scheduler) {
    kfree(scheduler);
}
