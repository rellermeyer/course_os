.text
    .global switch_context, store_context

switch_context:
    // Set the CPSR register.
    ldr r1, [r0, #16]
    msr cpsr, r1
    // Load the stack pointer.
    ldr sp, [r0, #52]
    // Push all other registers on the stack.
    ldr r1, [r0, #64]
    push {r1}
    ldr r1, [r0, #60]
    push {r1}
    ldr r1, [r0, #56]
    push {r1}
    ldr r1, [r0, #48]
    push {r1}
    ldr r1, [r0, #44]
    push {r1}
    ldr r1, [r0, #40]
    push {r1}
    ldr r1, [r0, #36]
    push {r1}
    ldr r1, [r0, #32]
    push {r1}
    ldr r1, [r0, #28]
    push {r1}
    ldr r1, [r0, #24]
    push {r1}
    ldr r1, [r0, #20]
    push {r1}
    ldr r1, [r0, #16]
    push {r1}
    ldr r1, [r0, #12]
    push {r1}
    ldr r1, [r0, #8]
    push {r1}
    ldr r1, [r0, #4]
    push {r1}
    ldr r1, [r0, #0]
    push {r1}

    // TODO: Enter user mode
    // cps #16

    // Load all register states.
    // Loading the PC register effectively jumps into the user program
    ldm sp!,{R0-R12,r14-pc}^

store_context:
    // TODO: somehow make sure to store the context, without overriding unstored registers.
    // The registers probably have been overriden just by calling this function.
    mov pc, lr
