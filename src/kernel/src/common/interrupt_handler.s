.data
.text

.global _handle_swi

// Subroutine definitions
.global _switch_to_usermode
.global _init
.global _userspace_test_program

#ifndef INTERRUPT_HANDLER_S
#define INTERRUPT_HANDLER_S
.include "src/scheduler/dispatcher.s"
#endif

// Disable interrupts? (IRQ, reset?)
_handle_swi:
    // Put swi number in r7
    ldr r7, [lr, #-4]           // SWI Instruction where we left off
    bic r7, r7, #0xff000000     // The value after the SWI instruction (swi call number)
    
    // Save lr when going into subroutine
    push {lr}
    bl syscall_handler 
    pop {lr}
    
    // Save the state of the process calling the software interrupt 
    _save_state_swi             
    // Schedule and swap the processes
    bl schedule
    // Load the next process
    _load_state_swi r0

_switch_to_usermode:
    MSR SPSR_c, #Mode_USR
    MOV r12, #0x8000
    add r12, #0x4
    MOVS pc, r12

_userspace_test_program:
    mov r12, pc
    mov r4, sp
    mov r0, #100
    mov r1, #0x1
    mov r2, #0x2
    mov r3, #0x3
    svc 0
    mov r4, sp
    mov r0, #69
    mov r1, #0x1
    mov r2, #0x2
    mov r3, #0x3
    svc 100


_init:
    // set the new pc
    mov r3, #0x8000
    add r3, #0x04

    // set the new sp
    mov r4, #0x8100
    add r4, #0x04
    init_state r3, r4
    add r5, #8
    str r4, [r5]

    bx lr
