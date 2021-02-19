.data
.text
.global _save_state
.global _init_state

// Store Execution State on the Stack
// !! We should adjust sp to point to the right table
_save_state:
    // Registers
    stm sp, {r4 - r11}^ // Dump all user(^) registers onto the stack

    // SpecialRegisters
    str r0, [sp, #-28]          // Put lr onto the stack
    str fp, [sp, #-32]          // Put fp onto the stack
    msr spsr, r0                // Load SPSR
    str r0, [sp, #-36]          // Put it on the stack
    mrc p15, 0, r0, c2, c0, 0   // Read 32-bit TTBR0 into R
    str r0, [sp, #-40]          // Put it on the stack

    mov r0, sp          // Set PCB Pointer as return value
    bx lr               // Return from subroutine

// TODO: Generate new PCB
_generate_context:
    // Registers
    stm sp, {r4 - r11}^ // Dump all user(^) registers onto the stack

    // SpecialRegisters
    str r0, [sp, #-28]          // Put lr onto the stack
    str fp, [sp, #-32]          // Put fp onto the stack
    msr spsr, r0                // Load SPSR
    str r0, [sp, #-36]          // Put it on the stack
    mrc p15, 0, r0, c2, c0, 0   // Read 32-bit TTBR0 into R
    str r0, [sp, #-40]          // Put it on the stack

    mov r0, sp          // Set PCB Pointer as return value
    bx lr               // Return from subroutine
