#include <asm_helper.h>
#include <process.h>
#include <registers.h>
#include <stdlib.h>
#include <string.h>
#include <thread.h>

/**
 * Create a new thread.
 */
Thread * create_thread(void * entry, Process * process) {
    Thread * thread = kmalloc(sizeof(Thread));

    thread->process = process;
    thread->state = Runnable;
    thread->registers = (struct Registers){0};

    // Allocate stack.
    size_t stack_end = 0x80000000 - (PAGE_SIZE * thread->process->threads->length);
    allocate_page(thread->process->vas, stack_end - PAGE_SIZE, false);
    thread->registers.SP = stack_end - 1;

    // Set instruction pointer.
    thread->registers.PC = (size_t)entry;
    thread->registers.CPSR = 0x16;

    // TODO: this makes returning from the thread main routine exit the thread
    //    thread->registers.LR = exit;

    return thread;
}

/**
 * Load a thread, and start executing.
 * Will never return.
 */
void load_thread(Thread * thread) {
    switch_to_vas(thread->process->vas);
    switch_context(&thread->registers);

    __builtin_unreachable();
}

/**
 * Overide the registers.
 */
void set_registers(Thread * thread, Registers * registers) {
    memcpy(&thread->registers, registers, 17 * 4);
}

/**
 * Free the thread.
 */
void free_thread(Thread * thread) {
    kfree((void *)thread->registers.SP);
    kfree(thread);
}
