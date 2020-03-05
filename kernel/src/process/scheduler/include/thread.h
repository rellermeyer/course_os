#ifndef COURSE_OS_THREAD_H
#define COURSE_OS_THREAD_H

#include "./process.h"
#include "./registers.h"

typedef enum ThreadState {
    Runnable,
    Running,
    Died,
} ThreadState;

typedef struct Thread {
    Registers registers;
    Process *process;
    ThreadState state;
} Thread;

Thread *create_thread(void *entry, Process *process);
void free_thread(Thread *thread);

#endif //COURSE_OS_THREAD_H
