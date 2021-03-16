.data
.text

.include "src/scheduler/dispatcher.s"

.global _handle_swi

// Disable interrupts? (IRQ, reset?)
_handle_swi:
    // Put swi number in r7
    ldr r7, [lr, #-4]           // SWI Instruction where we left off
    bic r7, r7, #0xff000000     // The value after the SWI instruction (swi call number)
    
    // Save lr when going into subroutine
    push {lr}
    bl syscall_handler 
    pop {lr}
    
    // Save the state of the process calling the software interrupt 
    _save_state_swi             
    // Schedule and swap the processes
    bl schedule
    // Load the next process
    _load_state_swi r0
