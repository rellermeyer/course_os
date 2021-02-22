#include <stdint.h>
#include <vas2.h>

/**
 * Registers r4-r10 should be saved, C depends on their values being the same when returning from
 * a subroutine.
 *
 * r0-r3 are scratch registers: They're used for as arguments to the syscall. Thus we don't need
 * them for the execution state
 *
 * ! Throwing an interrupt, however, is not a subroutine. Thus they should be saved, since
 * they can hold any data that might be important to execution.
 *
 * Registers r11-r15 are special registers, and especially important when restoring the state of the
 * process
 *
 * r11 - fp - Frame Pointer: Points to the bottom of the current stack; tracks the boundary of the
 * stack r12 is another scratch register (ip - Intra Procedure call register). This might be
 * important to execution r15 - pc - Program Counter: The memory address of the next instruction
 * cpsr - Current Program Status Register: Contains all the status flags
 *
 * These registers can be omitted:
 * r13 - sp - Stack Pointer: Point to the top of the current stack (of the subroutine). Since this
 *                           struct will be pushed directly onto the stack, and we have to return a
 *                           reference to it, we can deduce where the end of the stack was.
 *
 * r14 - lr - Link Register: Contains the value of the program counter + 4 bytes
 *                           (the next instruction), from where the program branched off.
 *                           ? This should be saved by the syscall upon triggering a SWI ?
 *
 * Note: These names are relative to the process in question. This being intuitive is up for debate
 * Ex. when the process is interrupted. The PC is stored in LP. We thus save the 'current' LP in the
 * variable PC.
 *
 * Note: When doing a syscall r7 is used to check which subroutine to call
 */

typedef struct {
    int32_t r6;
    union {
        int32_t r7;
        int32_t sys;
    };
    int32_t r8;
    int32_t r9;
    int32_t r10;
    union {
        uint32_t r11;
        uint32_t fp;
    };
    union {
        uint32_t r12;
        uint32_t ir;
    };
    union {
        uint32_t r14;
        uint32_t lr;
    };
    union {
        uint32_t r15;
        uint32_t pc;
    };
    uint32_t cpsr;
} Registers;

/**
 * The execution state should contain all the information to resume a process.
 * TODO: Add more attributes, like floating point registers
 * TODO: Page table or vas2 struct? Where do we get this from?
 */
typedef struct {
    Registers registers;
    //    struct * vas2 vas;
    //    void * (union vas; l1_page_table);
    void * l1_page_table;
} ExecutionState;

void _save_state(void);