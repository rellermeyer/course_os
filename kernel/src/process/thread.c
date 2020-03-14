#include <stdlib.h>
#include <thread.h>
#include <process.h>
#include <registers.h>
#include <asm_helper.h>

/**
 * Create a new thread.
 */
Thread *create_thread(void *entry, Process *process) {
    Thread *thread = kmalloc(sizeof(Thread));

    thread->process = process;
    thread->state = Runnable;
    thread->registers = (struct Registers){0};

    // Allocate stack.
    size_t stack_end = 0x80000000 - (PAGE_SIZE * thread->process->threads->length);
    allocate_page(thread->process->vas, stack_end, false);
    thread->registers.SP = stack_end + PAGE_SIZE - 1;

    // Set instruction pointer.
    thread->registers.PC = (size_t) entry;

    // TODO: this makes returning from the thread main routine exit the thread
//    thread->registers.LR = exit;

    return thread;
}

/**
 * Load a thread, and start executing.
 * Will never return.
 */
void load_thread(Thread *thread) {
    DEBUG("SWITCHING THREAD");

    switch_to_vas(thread->process->vas);
    switch_context(&thread->registers);

    __builtin_unreachable();
}

/**
 * Free the thread.
 */
void free_thread(Thread *thread) {
    kfree((void *) thread->registers.SP);
    kfree(thread);
}
