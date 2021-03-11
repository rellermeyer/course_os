.data
.text

.include "src/scheduler/dispatcher.s"

.global _handle_swi

_handle_swi:
    // Disable interrupts? (IRQ, reset?)
    bl syscall_handler 

    _save_state                 // save the state of the process calling the software interrupt (put in r4)
    ldr r7, [lr, #-4]           // SWI Instruction where we left off
    bic r7, r7, #0xff000000     // The value after the SWI instruction (swi call number)
    
    // Store result in executionState
    mov r0, r4
    bl schedule

    _load_state_swi r0
