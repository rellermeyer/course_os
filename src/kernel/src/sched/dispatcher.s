.data
.text
.global _save_state
.global _init_state

/**
 * Saves the context in an ExecutionState at the location of the stack pointer of the calling process
 *
 * Upon callin the subroutine, the old Link Register would normally be saved on the stack. We're
 * Gonna store the execution state anyway, so we'll make this the first argument.
 */
_save_state:
    // TODO: We might need to switch back to the user address space here
    // General purpose registers + fp (r11) + r12
    stm sp, {r4 - r13}^ // Dump all user(^) registers onto the stack

    str r1, [sp, #+36]           // Put lr onto the stack -> is is loading the value at addres r0?
    mrs r12, spsr                // Load SPSR
    str r12, [sp, #+40]          // Put it on the stack
    // TODO: Is this the l1 page table?
    mrc p15, 0, r12, c2, c0, 0   // Read 32-bit TTBR0 into R
    str r12, [sp, #+44]          // Put it on the stack

    // TODO: This is where we would switch back the kernel address space?
    mov r0, sp          // Set pointer to ExecutionState as return value
    bx lr               // Return from subroutine,

// TODO: Generate new PCB
_generate_context:
    nop
    bx lr
