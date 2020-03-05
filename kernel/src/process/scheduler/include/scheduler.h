#ifndef COURSE_OS_SCHEDULER_H
#define COURSE_OS_SCHEDULER_H

#include <thread.h>

typedef struct Scheduler {
    Thread *current_thread;
} Scheduler;

Scheduler *create_scheduler();
void free_scheduler(Scheduler *scheduler);

#endif //COURSE_OS_SCHEDULER_H
