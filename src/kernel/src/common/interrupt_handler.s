.data
.text

.global handle_swi
.global handle_irq

.global save_state_irq
.global load_state_irq
.global get_previous_sp
.global save_to_previous_sp

.extern chipset

// Subroutine definitions
.global _switch_to_usermode
.global _init
.global _userspace_test_program

#ifndef INTERRUPT_HANDLER_S
#define INTERRUPT_HANDLER_S
.include "src/scheduler/dispatcher.s"
#endif

.macro enter_priviledged_previous_mode
    push {r0}               // save r0 because it will be used to store the current mode

    // Find out from which mode we interrupted
    mrs r0, spsr
    and r0, #0x1f          // Mask off the Mode bits [4:0]

    // Move to target mode (SYS if we came from user mode)
    mrs r12, cpsr
    and r12, #0b11100000
    cmp r0, #Mode_USR
    orreq r12, r0
    orrne r12, #Mode_SYS

    pop {r0}                // retrieve the previous value of r0

    msr cpsr_c, r12
.endm

.macro enter_mode mode
    mrs r12, cpsr
    and r12, #0b11100000
    orr r12, \mode

    msr cpsr_c, r12
.endm

// Disable interrupts? (IRQ, reset?)
handle_swi:
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


handle_irq:
    push {lr}                   // Save lr for the execution state, since it's gonna be lost when branching

    // The subroutine is gonna do all the restoring, but unlike SWI won't return the 
    // Execection State (sp), but save it in the banked sp to make sure we can access it when
    // going though all the different C functions
    bl save_state_irq

    push {r0, r1}
    add lr, pc, #8              // save pc + 8 to lr
    ldr r0, =chipset
    add r0, #16
    ldr r1, [r0]

    pop {r0}
    blx r1
    pop {r1}
    //ldr pc, r12         // call chipset.handle_irq()
    b load_state_irq

save_state_irq:
    // TODO: Disable interrupts (VEWY INPOTRANT)
    cpsid if

    // Save all non-banked registers on current stack
    stmfd sp!, {r0-r12}

    // Save spsr onto current stack
    mrs r12, spsr
    stmfd sp!, {r12}

    // Save a reference to the current stack
    mov r1, sp

    enter_priviledged_previous_mode

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
    //msr cpsr_c, #Mode_IRQ          // reenables interrupts
    //mrs r12, cpsr
    //and r12, #0b11100000
    //orr r12, #Mode_IRQ

    //msr spsr, r12

    //push {lr}
    enter_mode #Mode_IRQ        // for some reason lr is 0?

    //ldmfd r1!, {lr}
    bx lr

load_state_irq:
    bl get_previous_sp

    // Load target pc into irq lr
    ldmfd r0!, {lr}

    // Jump to that mode (system if it's the user_mode)
    enter_priviledged_previous_mode

    // Restore the banked registers of the target mode
    mov sp, r0                  // Update after getting lr from other mode
    
    ldmfd sp!, {lr}             

    ldmfd sp!, {r9-r12}         // Load r10-12, and spsr (is in r9 ?????)
    // TODO enable interrupts in the other mode
    msr spsr_cxsf, r9           // Restore spsr

    ldmfd sp!, {r0-r9}          // Restore rest of the general purpose registers

    // Jump back to irq mode
    msr cpsr_c, #Mode_IRQ


    cpsie if
    // Resume program using movs
    movs pc, lr


get_previous_sp:
    enter_priviledged_previous_mode 

    // Get sp, and return to IRQ (Should be any mode)
    mov r1, sp
    msr cpsr_c, #Mode_IRQ

    mov r0, r1 
    bx lr

save_to_previous_sp:
    enter_priviledged_previous_mode 

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
