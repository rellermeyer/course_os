.text

// Subroutine definitions

// Value to store in cpsr when you want to switch to user mode
.equ Mode_USR, 0x10 

/**
    Save state will save the state of a process (execution state) onto the stack of said process, and return the address pointing to the top of the stack.

    It wil take 2 paramters:
        r4: The current user stack pointer 
        r0: The current user program counter 
 
    Execution state:
        The process state essentially consists of all the registers 
        
        - r4-r12 (general purpose registers)
        - lr
        - sp
        - pc

    Return values:
        The return value is the top of the user stack, including the execution state.

    Note: Currently the context switcher only works for the SVC interrupt.
          The code has to be rewritten to not require any parameters.
        
    Note: The return value is saved in r4 instead of r0. 
          We use registers r0-r3 for the SYSCALL parameters. 
          And we don't know which interrupt we're currently handling


    An important thing to remember while reading this code is that there are certain registers that are 'banked'
    when switching to a different mode. This means that they will not be overwritten when switching from one mode
    to another. It is thus also important that we save the user banked registers (sp, and lr), and not those of the
    current mode
**/
.macro _save_state
    mov r1, sp                  // Save current mode stack in r1
    mov sp, r4                  // Now relocate stack pointer to user stack (that way we can use macros such as push/pop)

    push {r0}                   // Store program counter 
    stmfd sp!, {r4 - r12}       // Store general purpose registers
    // TODO: Save user banked LR onto the stack

    mrs r2, spsr                // Load SPSR into a register (cannot be immeadiatly loaded into memory)
    push {r2}                   // Store it

    mov r4, sp                  // Save user space stack pointer back to r4
    mov sp, r1                  // Restore current mode stack
    // bx lr                       // Return, remember that r4 is the pointer to the PCB
.endm

/**
    Loads an existing Execution state, resuming execution of a process.
    This is essentially the save state in reverse.
 */
.macro _load_state
    mov r1, sp                      // Save interrupt stack into r1 (never lose a reference)
    mov sp, r4                      // Set our stack pointer to user space

    pop {r2}                        
    msr spsr_cxsf, r2               // Load the program state
    
    ldmfd sp!, {r4 - r12}           // Load general purpose registers
    pop {lr}                        // Load the user program counter into lr (will be stored into pc when returning)

    mov sp, r1                      // Restore the current mode stack pointer
    movs pc, lr                     // Set pc back to user program, and load the spsr into cpsr (the 's' in movs)
.endm

/**
    Initializes a process with an zeroed PCB, and putting the Program Counter at the first instruction.
 */
_init_state:
    // TODO: Generate new PCB
    bx lr


_switch_to_usermode:
    MSR SPSR_c, #Mode_USR
    MOV r12, #0x8000
    add r12, #0x4
    MOVS pc, r12

_userspace_test_program:
    mov r12, pc
    add r12, #0x100
    mov sp, r12
    mov r4, sp
    mov r0, #100
    mov r1, #0x1
    mov r2, #0x2
    mov r3, #0x3
    svc 0
    mov r4, sp
    mov r0, #100
    mov r1, #0x1
    mov r2, #0x2
    mov r3, #0x3
    svc 100

