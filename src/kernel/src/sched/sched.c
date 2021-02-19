#include <sched.h>

ExecutionState * save_context(void * sp, void * lr, void * fp) {
    // I'm for now assuming this includes r10
    asm volatile ("stm r0, {r4 - r11}^");
    asm volatile ("str r1, [r0, #-28]");
    asm volatile ("str r2, [r0, #-32]");
    asm volatile ("msr spsr, r1");
    asm volatile ("str r1, [r0, #-36]");

    return sp;
}
