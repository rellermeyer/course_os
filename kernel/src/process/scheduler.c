#include <stdlib.h>
#include <stdio.h>

#include "./include/scheduler.h"

Scheduler *create_scheduler() {
    Scheduler *scheduler = kmalloc(sizeof(Scheduler));
    scheduler->current_thread = NULL;

    return scheduler;
}

void enable_scheduler(Scheduler *scheduler) {
    kprintf("Starting scheduler..\n");
    load_thread(scheduler->current_thread);
}

void add_process_to_scheduler(Scheduler *scheduler, Process *process) {
    scheduler->current_thread = vpa_get(process->threads, 0);
}

void free_scheduler(Scheduler *scheduler) {
    kfree(scheduler);
}
