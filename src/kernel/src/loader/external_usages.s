
.text

#ifndef INTERRUPT_HANDLER_S
#define INTERRUPT_HANDLER_S
.include "../src/scheduler/dispatcher.s"
#endif

.global call_init_state

// This function expects the following arguments:
// 1) r0 - the start address for the new process
// 2) r1 - the initial stack pointer for the new process
call_init_state:
    push {lr}       // Save the link register, containing the return address, on the stack
    mov r3, r0      // Move the arguments to r3 and r4 to
    mov r4, r1      // make sure no problems with substitution happen in the macro
    mov r5, r2

    init_state r3, r4   // Use the macro to initialize the stack of the new process

    add r5, #8          // r5 holds the address of a PCB
    str r4, [r5]        // Store the pointer to the execution context
    pop {pc}        // Return from the subroutine: pc <- pushed lr
