.data
    .equ PCB_SIZE, 44
.text
.global _save_state
.global _init_state

/**
 * Saves the context in an ExecutionState at the location of the stack pointer of the calling process
 * Register contents on entry:
 *    
 *    R0 - R3 - parameter registers for the SWI handler, do not need to be saved

 *    R4 -> SP_user_mode
 *    R5 -> LR_user_mode
 *    
 *    R7 - R11 - just save

 *    FP -> FP_user_mode
 *    LR -> PC_user_mode
 *    SPSR -> CPSR_user_mode
 *
 */
_save_state:
    // TODO: We might need to switch back to the user address space here
    sub r4, #PCB_SIZE            // Allocate memory
    stm r4!, {r6 - r13}^         // Dump all user(^) registers onto the stack

    str r5, [r4, #+4]!           // Put lr onto the stack 
    str lr, [r4, #+4]!           // Put pc onto the stack
    
    mrs r12, spsr                // Load SPSR (cannot be immeadiatly loaded into memory)
    str r12, [r4, #+4]!          // Put it on the stack
    mrc p15, 0, r12, c2, c0, 0   // Read 32-bit TTBR0 into r12 (l1 page table of the calling process)
    str r12, [r4, #+4]!          // Put it on the stack

    // TODO: This is where we would switch back the kernel address space?
    sub r4, #PCB_SIZE            // Set pointer to ExecutionState

/**
 * Initializes a process with an zeroed PCB, and putting the Program Counter at the first instruction.
 */
_init_state:
    // TODO: Generate new PCB
    nop
    bx lr

/**
 * Loads and existing PCB, resuming execution of a process
 */
_load_state:
    // TODO: Load existing PCB
    nop
    bx lr
