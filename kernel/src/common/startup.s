
.global _Reset

_Reset:
    // Disable other cores
    mrc p15, #0, r1, c0, c0, #5
    and r1, r1, #3
    cmp r1, #0
    bne loop

    // Move the interrupt vector tables to 0x80000000
    ldr r0, =0xE000ED08
    ldr r1, =0x80000000
    str r1, [r0]


    ldr sp, =EARLY_KERNEL_STACK_TOP // Set the kernel/SVC stack
    push {r0-r11}

    // Identity map 0x00000000 (the kernel)
    ldr r0, =0x4000
    mov r1, #0
    orr r1, #0x02
    orr r1, #0x400
    str r1, [r0]

    // Map the lowest 1Mb to 0x8000000
    ldr r0, =__KERNEL_BASE
    lsr r0, #20
    lsl r0, #2
    ldr r1, =0x4000
    add r0, r1, r0

    mov r1, #0
    orr r1, #0x02
    orr r1, #0x400
    str r1, [r0]

    mov r0, #0
    // invalidate caches
    mcr p15, 0, r0, c7, c7, 0
    // invalidate tlb
    mcr p15, 0, r0, c8, c7, 0
    // data sync barrier
    mcr p15, 0, r0, c7,c10, 4

    // set domains
    mov r2, #0x01
    mcr p15, 0, r2, c3, c0, 0

    ldr r0, =0x4000
    // Give the pagetable addr to the MMU
    mcr p15, 0, r0, c2, c0, 0 // Table 0
    mcr p15, 0, r0, c2, c0, 1 // Table 1

    eor r0, r0, r0            // Zero r0
    mcr p15, 0, r0, c2, c0, 2 // Enable page walks on both page tables.

    mrc p15, 0, r3, c1, c0, 0
    orr r3, #0x800000
    orr r3, r3, #0x1           // Enable MMU bit
    mcr p15, 0, r3, c1, c0, 0 // Set p15

    pop {r0-r11}
    ldr sp, =KERNEL_STACK_TOP // Set the kernel/SVC stack
    push {r0-r11}

    // Setup
    bl stacks

    // Jumpt to the start of the kernel
    bl start

loop:
    WFI
    B loop


.bss
EARLY_KERNEL_STACK_BOTTOM:
    .space 128
EARLY_KERNEL_STACK_TOP:
