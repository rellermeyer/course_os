.data
.text

.global _handle_swi

_handle_swi:
    push {r0, r1, r2, r3, lr}   // Save arguments onto the stack
    mov r0, lr              // Set lr as first argument
    bl _save_state          // Call save_state subroutine
    pop {r0, r1, r2, r3, lr}    // Restore arguments

    ldr r7, [lr, #-4]          // SWI Instruction where we left off
    bic r7, r10, #0xff000000   // The value after the SWI instruction (swi call number)

    bl syscall_handler

    b _load_state
