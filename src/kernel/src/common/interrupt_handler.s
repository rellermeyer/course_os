.data
.text

.include "src/sched/dispatcher.s"

.global _handle_swi

_handle_swi:
    push {r0, r1, r2, r3, lr}   // Save arguments onto the stack
    mov r0, lr                  // Set lr as first argument
    _save_state                 // save the state of the process calling the software interrupt
    pop {r0, r1, r2, r3, lr}    // Restore arguments

    ldr r7, [lr, #-4]           // SWI Instruction where we left off
    bic r7, r7, #0xff000000     // The value after the SWI instruction (swi call number)

    bl syscall_handler

    _load_state
