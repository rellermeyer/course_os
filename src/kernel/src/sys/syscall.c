#include "syscall.h"
#include "stdio.h"

// if error, return -1
int syscall(int sys, ...) {
    va_list args;
    va_start(args, sys);

    // variables that can be passed to the SWI
    int r0 = 0, r1 = 0, r2 = 0, r3 = 0;

    // TODO: the interrupt handler should be able to get the pid from the scheduler?
    switch(sys) {
        case SYS_fork:
            // TODO: should write the pid of the (newly-created) child process where r0 is pointing
            // (ask the scheduler?) 

            asm("MOV r7, %0":: "r" (sys));
            int* pid = va_arg(args, int*);
            asm("MOV r0, %0":: "r"(pid));
            asm("MOV r7, %0":: "r" (sys));
            asm("PUSH {lr}"); // need to save the return address to the stack
            asm("SWI 0x0");
            asm("POP {lr}"); // pop lr from the stack to return to whoever called syscall()

            break;
        case SYS_kill:
            r0 = va_arg(args, int);
            r1 = va_arg(args, int);

            asm("MOV r7, %0":: "r" (sys));
            asm("MOV r0, %0":: "r" (r0));
            asm("MOV r1, %0":: "r" (r1));
            asm("PUSH {lr}"); // need to save the return address to the stack
            asm("SWI 0x0");
            asm("POP {lr}"); // pop lr from the stack to return to whoever called syscall()

            break;
        case SYS_exit:
            r0 = va_arg(args, int);

            asm("MOV r7, %0":: "r" (sys));
            asm("MOV r0, %0":: "r" (r0));
            asm("PUSH {lr}"); // need to save the return address to the stack
            asm("SWI 0x0");
            asm("POP {lr}"); // pop lr from the stack to return to whoever called syscall()
            break;

        case SYS_sleep:
            r0 = va_arg(args, int);

            asm("MOV r7, %0":: "r" (sys));
            asm("MOV r0, %0":: "r" (r0));
            asm("PUSH {lr}"); // need to save the return address to the stack
            asm("SWI 0x0");
            asm("POP {lr}"); // pop lr from the stack to return to whoever called syscall()

            break;

        case SYS_join:
            asm("MOV r7, %0":: "r" (sys));
            asm("PUSH {lr}"); // need to save the return address to the stack
            asm("SWI 0x0");
            asm("POP {lr}"); // pop lr from the stack to return to whoever called syscall()

            break;
        case SYS_dummy:
            // only for testing

            r0 = va_arg(args, int);
            r1 = va_arg(args, int);
            r2 = va_arg(args, int);
            r3 = va_arg(args, int);
            asm("MOV r7, %0":: "r" (sys));
            asm("MOV r0, %0":: "r" (r0));
            asm("MOV r1, %0":: "r" (r1));
            asm("MOV r2, %0":: "r" (r2));
            asm("MOV r3, %0":: "r" (r3));
            asm("PUSH {lr}"); // need to push the return address to the stack
            // for some reason the software interrupt changes lr to an address in software_interrupt_handler()
            // by saving the previous lr before invoking the software interrupt handler,
            // we can return to whatever called syscall()
            asm("SWI 0x0");
            asm("POP {lr}"); // pop lr from the stack to return to whoever called syscall()

            break;
        default:
            break;
    }

    va_end(args);
    return 0;
}
