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
    Process * process;
    ThreadState state;
} Thread;

Thread * create_thread(void * entry, Process * process);
void load_thread(Thread * thread);
void store_thread(Thread * thread);
void free_thread(Thread * thread);
void set_registers(Thread * thread, Registers * registers);

#endif  // COURSE_OS_THREAD_H
