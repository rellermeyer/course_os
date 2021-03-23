.data
.text

.global _handle_swi

.global save_state_irq
.global load_state_irq
.global enter_priviledged_previous_mode
.global get_previous_sp
.global save_to_previous_sp

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
    push {r7}                   // Save r7
    ldr r7, [lr, #-4]           // SWI Instruction where we left off
    bic r7, r7, #0xff000000     // The value after the SWI instruction (swi call number)
    
    // Save lr when going into subroutine
    push {lr}
    bl syscall_handler 
    pop {lr} 

    pop {r7}                    // Restore r7
    
    // Save the state of the process calling the software interrupt 
    _save_state_swi             
    // Schedule and swap the processes
    bl schedule
    // Load the next process
    _load_state_swi r0

save_state_irq:
    // TODO: Disable interrupts (VEWY INPOTRANT)

    // Save all non-banked registers on current stack
    stmfd sp!, {r0-r12}

    // Save spsr onto current stack
    mrs r12, spsr
    stmfd sp!, {r12}

    // Save a reference to the current stack
    mov r1, sp

    push {lr}
    bl enter_priviledged_previous_mode
    pop {lr}
    
    // Move everything from irq stack to target mode stack
    ldmfd r1!, {r3-r11}         // actually r0-r9 (I think)
    stmfd sp!, {r3-r11}         // r1 contains target mode sp

    ldmfd r1!, {r3-r6}          // actually r10-r12, and spsr
    stmfd sp!, {r3-r6}          

    stmfd sp!, {lr}             // Target mode lr
    
    ldmfd r1!, {r2}             // Target mode pc 
    adds r2, #-4                // Compensate, historic reasons don't ask me why
    stmfd sp!, {r2}             // Actually put it on there

    // Return to irq, and resume execution
    msr cpsr_c, #Mode_IRQ
    bx lr

load_state_irq:
    bl get_previous_sp

    // Load target pc into irq lr
    ldmfd r0!, {lr}

    // Jump to that mode (system if it's the user_mode)
    push {lr}
    bl enter_priviledged_previous_mode
    pop {lr}

    // Restore the banked registers of the target mode
    mov sp, r0                  // Update after getting lr from other mode
    
    ldmfd sp!, {lr}             

    ldmfd sp!, {r9-r12}         // Load r10-12, and spsr (is in r9 ?????)
    msr spsr_cxsf, r9           // Restore spsr

    ldmfd sp!, {r0-r9}          // Restore rest of the general purpose registers

    // Jump back to irq mode
    msr cpsr_c, #Mode_IRQ

    // Resume program using movs
    movs pc, lr

enter_priviledged_previous_mode:
    // Find out from which mode we interrupted
    mrs r0, spsr
    and r0, #0x1f          // Mask off the Mode bits [4:0]
    cmp r0, #Mode_USR

    // Move to target mode (SYS if we came from user mode)
    msreq cpsr_c, r0            
    msrne cpsr_c, #Mode_SYS

    bx lr

get_previous_sp:
    push {lr}
    bl enter_priviledged_previous_mode 
    pop {lr}
   
    // Get sp, and return to IRQ (Should be any mode)
    mov r1, sp
    msr cpsr_c, #Mode_IRQ

    mov r0, r1 
    bx lr

save_to_previous_sp:
    push {lr}
    bl enter_priviledged_previous_mode 
    pop {lr}
   
    // Get sp, and return to IRQ (Should be any mode)
    mov sp, r0
    msr cpsr_c, #Mode_IRQ

    bx lr

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
