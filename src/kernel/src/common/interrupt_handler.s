.data
.text

.include "src/sched/dispatcher.s"

.global _handle_swi

_handle_swi:
    // Disable interrupts? (IRQ, reset?)
    _save_state                 // save the state of the process calling the software interrupt

    ldr r7, [lr, #-4]           // SWI Instruction where we left off
    bic r7, r7, #0xff000000     // The value after the SWI instruction (swi call number)

    bl syscall_handler
    bl getNext

    _load_state_swi r4
