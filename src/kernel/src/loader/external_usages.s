
.text

#ifndef INTERRUPT_HANDLER_S
#define INTERRUPT_HANDLER_S
.include "src/scheduler/dispatcher.s"
#endif

.global call_init_state

// This function expects the following arguments:
// 1) r0 - the start address for the new process
// 2) r1 - the initial stack pointer for the new process
call_init_state:
    mov r3, r0
    mov r4, r1
    init_state r3, r4

