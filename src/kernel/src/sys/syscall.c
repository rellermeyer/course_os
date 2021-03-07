#include "syscall.h"
#include "stdio.h"

#define SVC(code) asm volatile ("svc %0" :: "I" (code))

// if error, return -1
int syscall(int sys, ...) {
    va_list args;
    va_start(args, sys);

    // variables that can be passed to the SWI
    int r0 = 0, r1 = 0, r2 = 0, r3 = 0;
    int ret = 0;

    switch (sys) {
        case SYS_fork:
            SVC(SYS_fork);
                
            break;
        case SYS_kill:
            r0 = va_arg(args, int);
            r1 = va_arg(args, int);

            asm volatile ("MOV r0, %0" ::"r"(r0));
            asm volatile ("MOV r1, %0" ::"r"(r1));
            SVC(SYS_kill);
            

            break;
        case SYS_exit:
            r0 = va_arg(args, int);

            asm volatile ("MOV r0, %0" ::"r"(r0));
            SVC(SYS_exit);
            break;

        case SYS_sleep:
            r0 = va_arg(args, int);

            asm volatile ("MOV r0, %0" ::"r"(r0));
            SVC(SYS_sleep);

            break;

        case SYS_join:
            SVC(SYS_join);

            break;
        case SYS_dummy:
            // only for testing
            r0 = va_arg(args, int);
            r1 = va_arg(args, int);
            r2 = va_arg(args, int);
            r3 = va_arg(args, int);

            asm volatile("MOV r0, %0" ::"r"(r0));
            asm volatile("MOV r1, %0" ::"r"(r1));
            asm volatile("MOV r2, %0" ::"r"(r2));
            asm volatile("MOV r3, %0" ::"r"(r3));

            SVC(SYS_dummy);

            break;
        default:
            break;
    }
    kprintf("Ended syscall\n");
    va_end(args);
    return ret;
}
