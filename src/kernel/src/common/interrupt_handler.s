.data
.text

.global _handle_swi

_handle_swi:
    push {r0, r1, r2, r3}   // Save arguments onto the stack
    mov r0, lr              // Set lr as first argument
    bl _save_state          // Call save_state subroutine
    pop {r0, r1, r2, r3}    // Restore arguments

    bl syscall_handler

    b _load_state
