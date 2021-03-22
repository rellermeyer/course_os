.text


// Value to store in cpsr when you want to switch to user mode
.equ Mode_USR, 0x10
.equ Mode_SYS, 0x1FU
.equ Mode_SVC, 0x13U

/**
    Save state will save the state of a process (execution state) onto the stack of said process, and return the address pointing to the top of the stack.

    Execution state:
        The process state essentially consists of all the registers

        - r0-r12 (general purpose registers)
        - lr
        - spsr
        - pc

    Return value:
        top of the user stack, including the execution state.


    An important thing to remember while reading this code is that there are certain registers that are 'banked'
    when switching to a different mode. This means that they will not be overwritten when switching from one mode
    to another. It is thus also important that we save the user banked registers (sp, and lr), and not those of the
    current mode
**/
.macro _save_state_swi
    // Save all the general puropse registers, and user lr
    msr cpsr_c, #Mode_SYS           // Enter system mode

    stmfd sp!, {r0 - r12}           // Save the user registers on to the user stack
    stmfd sp!, {lr}                 // Save the user registers on to the user stack

    msr cpsr_c, #Mode_SVC           // Return to svc mode
    
    // Move the banked user's sp into r0
    stmfd sp!, {sp}^                // Save the user sp onto the interrupt stack
    ldmfd sp!, {r0}                 // And load it into r0

    // Save program status register on the process stack
    mrs r12, spsr                   // Load SPSR into a register (cannot be immeadiatly loaded into memory)
    stmfd r0!, {r12}                // Store it onto the stack
    
    stmfd r0!, {lr}                 // Save user PC onto the stack
.endm

/**
    Loads an existing Execution state, resuming execution of a process.
    This is essentially the save state in reverse.
 */
.macro _load_state_swi user_sp
    // Entry point into user proces
    ldmfd \user_sp, {lr}             // Save user pc in current mode lr

    // Enter system mode, and align sp to target process
    msr cpsr_c, #Mode_SYS           // Enter system mode
    mov sp, \user_sp                // Set stack pointer to right position

    // Restore program status register
    ldmfd sp!, {r12}                       // Retrieve the SPSR from the user stack
    msr spsr_cxsf, r12

    // Restore general purpose registers and lr
    ldmfd sp!, {lr}                 // Restore the user registers r0-r12 (prepare to return to user mode)
    ldmfd sp!, {r0-r12}             // Restore the user registers r0-r12 (prepare to return to user mode)

    // Return to user process
    msr cpsr_c, #Mode_SVC           // Return to svc mode
    movs pc, lr                     // Jump to user code, and load spsr
.endm

.macro init_state new_pc, new_sp
    // Enter System mode
    msr cpsr_c, #Mode_SYS
    mov sp, \new_sp         // Set sp to new stack

    // Set up for loop
    mov r1, #13
    mov r2, #0

// Push 13 (r0-r12, lr) zeroes onto the stack
.loop\@:
    push { r2 }             // Push a zero
    add r1, #-1             // Count
    cmp r1, #0          
    bne .loop\@             // Continue while it's not zero

    // Push new cpsr 
    mov r2, #0b10010000     // IRQs are enabled and FIQs aren't [100], mode is user mode [10000]
    push { r2 }             // Put it on the stack

    push { \new_pc }        // Put the new user program counter on the stack
  
    // Return
    mov \new_sp, sp         // Update the 'new_sp', to point to the ExectionState
    msr cpsr_c, #Mode_SVC   // Return to system mode
.endm

.macro save_state_rsi
    nop
    // Save all non-banked registers on current stack
    // Find out from which mode we interrupted
    // Retrieve the sp, and lr from that mode (lr should be pushed last, since that's the first we want to retrieve when loading)
    // Transfer all registers from the irq stack, onto the target mode stack (what do we do if we've come from an irq?)
    // Update target sp to be the new stack top (including the execution state)
.endm

.macro load_state_rsi target_sp
    nop
    // Load target pc into irq lr
    // Jump to that mode (system if it's the user_mode)
    // Restore the banked registers of the target mode
    // Then restore the other registers
    // Jump back to irq mode
    // Resume program using movs
.endm
