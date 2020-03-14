#ifndef COURSE_OS_SCHEDULER_H
#define COURSE_OS_SCHEDULER_H

#include <thread.h>

typedef struct Scheduler {
    Thread *current_thread;
} Scheduler;

Scheduler *create_scheduler();
void enable_scheduler(Scheduler *scheduler);
void add_process_to_scheduler(Scheduler *scheduler, Process *process);
void free_scheduler(Scheduler *scheduler);

#endif //COURSE_OS_SCHEDULER_H
