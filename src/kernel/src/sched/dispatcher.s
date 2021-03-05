.data
.text
.global _save_state
.global _load_state
.global _switch_to_usermode
.global _userspace_test_program

.equ Mode_USR, 0x10 // User
/**
    Saves the context in an ExecutionState at the location of the stack pointer of the calling process
    Register contents on entry:
    
       R0 -> pc in user space, lr from interrupt 'space', argument so does not have to be stored
       R1, R2 -> Scratch registers, do not have to be stored
    
       R4 -> SP_user_mode
       R5 -> LR_user_mode
    
       R7 - R11 - just save

       FP -> FP_user_mode
       LR -> PC_user_mode (Our first argument)
       SPSR -> CPSR_user_mode

    It must be noted that a struct in C is interpreted as follows: 
              the first element in the struct is the top element of the stack.
              Since the stack is LIFO, we must add all enties in reverse order
 */
_save_state:
    // TODO: We might need to switch back to the user address space here
    mov r1, sp                  // Save Interrupt stack in r1
    mov sp, r4                  // Now relocate stack pointer to user stack (that way we can use macros such as push/pop)

    push {r0} // pc
    // Dump registers r6-r12, pc and lr onto the stack (Full Descending)
    stmfd sp!, {r6 - r12}

    mrs r2, spsr                // Load SPSR (cannot be immeadiatly loaded into memory)
    push {r2}                   // Put it on the stack

    // TODO: This is where we would switch back the kernel address space?
    mov r4, sp                   // Save user space stack pointer back to r4
    mov sp, r1                   // Restore Interrupt stack
    bx lr                        // Return, remember that r4 is the pointer to the PCB

/**
    Loads and existing PCB, resuming execution of a process
 */
_load_state:
    // TODO: Load existing PCB
    mov r1, sp                      // Save interrupt stack into r1 (never lose a reference)
    mov sp, r4                      // Set our stack pointer to user space

    pop {r2}                        
    msr spsr_cxsf, r2                    // Load the program state
    
    ldmfd sp!, {r6 - r12}     // Load all user(^) general purpose registers, pc and lr
    pop {pc}

/**
    Initializes a process with an zeroed PCB, and putting the Program Counter at the first instruction.
 */
_init_state:
    // TODO: Generate new PCB
    bx lr


_switch_to_usermode:
    MSR SPSR_c, #Mode_USR
    // jump to the _userspace_test_program that for now just calls an empty software interrupt
    MOV r12, #0x8000
    MOVS pc, r12

_userspace_test_program:
    swi 0x0

