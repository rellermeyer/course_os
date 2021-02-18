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
            // TODO: should return (in r0) pid of the (newly-created) child process
            // (ask the scheduler?) 

			asm("MOV r7, %0":: "r" (sys));
			asm("SWI 0x0");

            break;
        case SYS_kill:
            // TODO

			r0 = va_arg(args, int);
			r1 = va_arg(args, int);
			asm("MOV r7, %0":: "r" (sys));
			asm("MOV r0, %0":: "r" (r0));
			asm("MOV r1, %0":: "r" (r1));
			//asm("MOV r2, %0": "+r" (r2));
			//asm("MOV r3, %0": "+r" (r3));
			asm("SWI 0x0");
            break;
        case SYS_exit:
            // TODO

			r0 = va_arg(args, int);
			asm("MOV r7, %0":: "r" (sys));
			asm("MOV r0, %0":: "r" (r0));
			asm("SWI 0x0");
            break;
        case SYS_dummy:
            // only for testing

			r0 = va_arg(args, int);
			r1 = va_arg(args, int);
			r2 = va_arg(args, int);
            // TODO: r3 is overwritten (same problem as in interrupt.c)
			r3 = va_arg(args, int);
			asm("MOV r7, %0":: "r" (sys));
			asm("MOV r0, %0":: "r" (r0));
			asm("MOV r1, %0":: "r" (r1));
			asm("MOV r2, %0":: "r" (r2));
			asm("MOV r3, %0":: "r" (r3));
			asm("SWI 0x0");
            /*
                for some reason calling:
                syscall(2, 1, 2, 3, 4);

                causes a kernel panic

                But executing the code instead (which does the same thing):
                int sys = 2, r0 = 1, r1 = 2, r2 = 3, r3 = 4;
                asm("MOV r7, %0":: "r" (sys));
                asm("MOV r0, %0":: "r" (r0));
                asm("MOV r1, %0":: "r" (r1));
                asm("MOV r2, %0":: "r" (r2));
                asm("MOV r3, %0":: "r" (r3));
                asm("SWI 0x0");

                works fine.
            */
            break;
        default:
            break;
    }

    va_end(args);
    return 0;
}
