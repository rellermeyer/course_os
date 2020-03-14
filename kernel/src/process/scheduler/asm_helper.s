.text
    .global switch_context

switch_context:
    ldr r1, [r0, #16]
    msr cpsr, r1
    ldr r13, [r0, #13]
    ldr r1, [r0, #15]
    push {r1}
    ldr r1, [r0, #0]
    push {r1}
    ldr r1, [r0, #1]
    push {r1}
    ldr r1, [r0, #2]
    push {r1}
    ldr r1, [r0, #3]
    push {r1}
    ldr r1, [r0, #4]
    push {r1}
    ldr r1, [r0, #5]
    push {r1}
    ldr r1, [r0, #6]
    push {r1}
    ldr r1, [r0, #7]
    push {r1}
    ldr r1, [r0, #8]
    push {r1}
    ldr r1, [r0, #9]
    push {r1}
    ldr r1, [r0, #10]
    push {r1}
    ldr r1, [r0, #11]
    push {r1}
    ldr r1, [r0, #12]
    push {r1}
    ldr r1, [r0, #14]
    push {r1}
    ; cps #16
    pop {r14}
    pop {r12}
    pop {r11}
    pop {r10}
    pop {r9}
    pop {r8}
    pop {r7}
    pop {r6}
    pop {r5}
    pop {r4}
    pop {r3}
    pop {r2}
    pop {r1}
    pop {r0}
    pop {r15}
