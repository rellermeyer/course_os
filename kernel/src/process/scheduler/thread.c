#include <stdlib.h>

#include "./include/thread.h"
#include "./include/process.h"
#include "./include/registers.h"
#include "./include/asm_helper.h"

#include "../../../../user/libc/arch/arm/syscall_arch.h"

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

void load_thread(Thread *thread) {
    kprintf("yeet55\n");
    /** // asm volatile(
    //     "msr cpsr, %16 \n" \
    //     "mov r13, %13 \n" \
    //     "push {%15} \n" \
    //     "push {%0} \n" \
    //     "push {%1} \n" \
    //     "push {%2} \n" \
    //     "push {%3} \n" \
    //     "push {%4} \n" \
    //     "push {%5} \n" \
    //     "push {%6} \n" \
    //     "push {%7} \n" \
    //     "push {%8} \n" \
    //     "push {%9} \n" \
    //     "push {%10} \n" \
    //     "push {%11} \n" \
    //     "push {%12} \n" \
    //     "push {%14} \n" \
    //     "cps #16 \n" \
    //     "pop r14 \n" \
    //     "pop r12 \n" \
    //     "pop r11 \n" \
    //     "pop r10 \n" \
    //     "pop r9 \n" \
    //     "pop r8 \n" \
    //     "pop r7 \n" \
    //     "pop r6 \n" \
    //     "pop r5 \n" \
    //     "pop r4 \n" \
    //     "pop r3 \n" \
    //     "pop r2 \n" \
    //     "pop r1 \n" \
    //     "pop r0 \n" \
    //     "pop r15 \n"
    //     ::
    //     "r"(thread->registers.R0),
    //     "r"(thread->registers.R1),
    //     "r"(thread->registers.R2),
    //     "r"(thread->registers.R3),
    //     "r"(thread->registers.R4),
    //     "r"(thread->registers.R5),
    //     "r"(thread->registers.R6),
    //     "r"(thread->registers.R7),
    //     "r"(thread->registers.R8),
    //     "r"(thread->registers.R9),
    //     "r"(thread->registers.R10),
    //     "r"(thread->registers.R11),
    //     "r"(thread->registers.R12),
    //     "r"(thread->registers.R13),
    //     "r"(thread->registers.R14),
    //     "r"(thread->registers.R15),
    //     "r"(thread->registers.CPSR)
    //     :
    //     "cc",
    //     "r0",
    //     "r1",
    //     "r2",
    //     "r3",
    //     "r4",
    //     "r5",
    //     "r6",
    //     "r7",
    //     "r8",
    //     "r9",
    //     "r10",
    //     "r11",
    //     "r12",
    //     "r13",
    //     "r14",
    //     "r15"
    // ); **/

    switch_context(thread->registers);

    // asm("mov r0, %0" :: "r"(thread->registers.R0));
    // asm("mov r1, %0" :: "r"(thread->registers.R1));
    // asm("mov r2, %0" :: "r"(thread->registers.R2));
    // asm("mov r3, %0" :: "r"(thread->registers.R3));
    // asm("mov r4, %0" :: "r"(thread->registers.R4));
    // asm("mov r5, %0" :: "r"(thread->registers.R5));
    // asm("mov r6, %0" :: "r"(thread->registers.R6));
    // asm("mov r7, %0" :: "r"(thread->registers.R7));
    // asm("mov r8, %0" :: "r"(thread->registers.R8));
    // asm("mov r9, %0" :: "r"(thread->registers.R9));
    // asm("mov r10, %0" :: "r"(thread->registers.R10));
    // asm("mov r12, %0" :: "r"(thread->registers.R12));
    // asm("mov r13, %0" :: "r"(thread->registers.R13));
    // asm("mov r14, %0" :: "r"(thread->registers.R14));
    kprintf("yeet1\n");
    // asm("push {%0}" :: "r"(thread->registers.R15));
    // asm("cps #16");
    __syscall0(0);
    // asm("pop {r15}");
    // asm("mov r15, %0" :: "r"(thread->registers.R15));
    // __syscall0(0);
    // __syscall1(18, (long int) &"yeeeeet");
    // kprintf("yeet2\n");
    // kprintf("yeet3\n");

	// register long r7 __asm__("r7") = n;
	// register long r0 __asm__("r0") = a;
	// __asm_syscall("r"(r7), "0"(r0));

    // kprintf("Done!!!!!!!\n");
    // __builtin_unreachable();
}

void free_thread(Thread *thread) {
    kfree((void *) thread->registers.SP);
    kfree(thread);
}
