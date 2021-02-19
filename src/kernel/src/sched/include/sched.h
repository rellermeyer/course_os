#include <stdint.h>

/**
 * Registers r4-r10 should be saved, C depends on their values being the same when returning from
 * a subroutine.
 *
 * r0-r3 are scratch registers: there is no guarantee that they will be the same after calling
 * a subroutine. Thus we don't save them.
 */
typedef struct {
    int32_t r4;
    int32_t r5;
    int32_t r6;
    int32_t r7; // Often used for indicating syscall number
    int32_t r8;
    int32_t r9;
    int32_t r10;
} SavedRegisters;

/**
 * Registers r11-r15 are special registers, and especially important when restoring the state of the process
 *
 * r11 - fp - Frame Pointer: Points to the bottom of the current stack; tracks the boundary of the stack
 * r14 - lr - Link Register: Contains the value of the program counter + 4 bytes
 *                           (the next instruction), from where the program branched off
 * r15 - pc - Program Counter: The memory address of the next instruction
 * cpsr - Current Program Status Register: Contains all the status flags
 *
 * These registers can be omitted:
 * r12 is another scratch register (ip - Intra Procedure call register), thus omitted
 * r13 - sp - Stack Pointer: Point to the top of the current stack (of the subroutine). Since this
 * struct will be pushed directly onto the stack, and we have to return a reference to it, we can
 * deduce where the end of the stack was. Thus omitted
 *
 * Note: These names are relative to the process in question.
 * Ex. when the process is interrupted. The PC is stored in LP. We thus save the 'current' LP in the
 * variable PC.
 */
typedef struct {
    uint32_t pc;
    uint32_t fp;
    uint32_t cpsr;
    uint32_t l1_table;
} SpecialRegisters;

/**
 * The execution state should contain all the information to resume a process. Keep in mind
 * TODO: Add more attributes, like floating point registers
 */
typedef struct {
    SavedRegisters registers;
    SpecialRegisters specialRegisters;
} ExecutionState;

/**
 * Saves the context in an ExecutionState at the location of the stack pointer of the calling process
 *
 * In calling the function save_context, the old Link Register, Stack Pointer, and Frame pointer are
 * lost, so they have to be given.
 */
ExecutionState * save_context(void * lr, void * sp, void * fp);
