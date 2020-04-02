#include <stdlib.h>
#include <stdio.h>

#include "./include/scheduler.h"

Scheduler *active_scheduler = NULL;

Scheduler *create_scheduler() {
    Scheduler *scheduler = kmalloc(sizeof(Scheduler));
    scheduler->current_thread = NULL;

    return scheduler;
}

void enable_scheduler(Scheduler *scheduler) {
    DEBUG("[SCHEDULER] Starting scheduler..");
    active_scheduler = scheduler;
    schedule_in(scheduler);
}

Scheduler *get_scheduler() {
    return active_scheduler;
}

void schedule_in(Scheduler *scheduler) {
    TRACE("[SCHEDULER] Scheduling a new thread..");
    load_thread(scheduler->current_thread);
}

void schedule_out(Scheduler *scheduler) {
    TRACE("[SCHEDULER] Scheduling out a thread..");
    store_thread(scheduler->current_thread);
}

void add_process_to_scheduler(Scheduler *scheduler, Process *process) {
    scheduler->current_thread = vpa_get(process->threads, 0);
}

void free_scheduler(Scheduler *scheduler) {
    kfree(scheduler);
}
