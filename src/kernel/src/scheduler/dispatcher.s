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
    push {r0, r1, r2, r3}       // Save arguments onto the stack

    // Save the banked user's sp into r0
    stmfd sp!, {sp}^            // Save the user stack onto the interrupt stack
    ldmfd sp!, {r0}

    //stmfd r0!, {r14}            // Save user PC onto the stack
    stmfd r0!, {r0 - r12, r14}      // Save the user registers on to the user stack

    //ldr r12, [sp, #4]!
    //stmfd r0!, {r12}

    mrs r12, spsr               // Load SPSR into a register (cannot be immeadiatly loaded into memory)
    stmfd r0!, {r12}            // Store it onto the stack
    mov r4, r0                  // Save the user sp in r4 to be used later to load the state

    pop {r0, r1, r2, r3}        // Restore arguments

.endm

/**
    Loads an existing Execution state, resuming execution of a process.
    This is essentially the save state in reverse.
 */
.macro _load_state_swi user_sp
    ldmfd \user_sp!, {r12}          // Retrieve the SPSR from the user stack
    //ldmfd \user_sp!, {r0}
    msr spsr_cxsf, r12

    // Restore the user registers from the stack
    mov sp, \user_sp                // Store the user sp into sp (in order to restore r4)

    // TODO add ^ when we need to return to user mode
    ldmfd sp!, {r0-r12}             // Restore the user registers r0-r12 (prepare to return to user mode)
    ldmfd sp!, {pc}^                // Restore the pc
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

