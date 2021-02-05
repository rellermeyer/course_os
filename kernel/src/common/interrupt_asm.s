.text
    .global software_interrupt_handler

software_interrupt_handler:
    // Push all registers to the stack
    push {r0} // Should be replaced with cpsr
    mov r0, sp
    sub r0, #4
    push {lr} // PC
    push {lr}
    push {r0} // SP
    stmdb sp!,{r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12}^

    add sp, #64
    ldr r1, [sp, #0] // restore r0
    mrs r0, spsr
    push {r0} // CPSR
    sub sp, #60

    push {r1} // push r0

    // Call the C swi handler
    mov r0, sp
    mov lr, #42 // Marker, in case it ever shows up as pc address.
    bl swi_handler
