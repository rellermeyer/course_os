#include <stdlib.h>

#include "./include/thread.h"
#include "./include/process.h"
#include "./include/registers.h"

Thread *create_thread(void *entry, Process *process) {
    Thread *thread = kmalloc(sizeof(Thread));

    thread->process = process;
    thread->state = Runnable;
    thread->registers = (struct Registers){0};

    // TODO: allocate proper pages.
    thread->registers.SP = (size_t) kmalloc(128);
    thread->registers.PC = (size_t) entry;

    return thread;
}

void free_thread(Thread *thread) {
    kfree((void *) thread->registers.SP);
    kfree(thread);
}
