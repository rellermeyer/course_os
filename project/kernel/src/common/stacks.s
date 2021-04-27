// This code creates multiple areas that are used as stacks for each of the
// CPU operating modes.

// Codes for the various execution modes
// https://heyrick.eu/armwiki/Processor_modes
.equ Mode_USR, 0x10 // User
.equ Mode_FIQ, 0x11 // Fast interrupt
.equ Mode_IRQ, 0x12 // Normal iterrupt
.equ Mode_SVC, 0x13 // Supervisor mode, this is the privileged moded for the OS (/Kernel)
.equ Mode_MON, 0x16 // Monitor, more info: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.prd29-genc-009492c/CACJBHJA.html
.equ Mode_ABT, 0x17 // Abort, for virtual memory and memory protection
.equ Mode_UND, 0x1B // Undefined,
.equ Mode_SYS, 0x1F // System, a hybrid between SVC and

.text

.global stacks
stacks:
   // FIQ Stack
    msr CPSR_c, #Mode_FIQ // Switch to FIQ Mode
    ldr sp, =FIQ_STACK_TOP // Set the FIQ stack pointer

    // IRQ Stack
    msr CPSR_c, #Mode_IRQ
    ldr sp, =IRQ_STACK_TOP

    // MON Stack
    msr CPSR_c, #Mode_MON
    ldr sp, =MON_STACK_TOP

    // ABT Stack
    msr CPSR_c, #Mode_ABT
    ldr sp, =ABT_STACK_TOP

    // UND Stack
    msr CPSR_c, #Mode_UND
    ldr sp, =UND_STACK_TOP

    // SYS Stack
    msr CPSR_c, #Mode_SYS
    ldr sp, =SYS_STACK_TOP

    // Switch back SVC/Kernel
    msr CPSR_c, #Mode_SVC
    add fp, sp, #0 // ???

    //MSR     CPSR_c, #Mode_USR
    //MOV     SP, R0
    //LDR sp, =SYS_STACK_TOP

    bx lr

.section .bss
.global KERNEL_STACK_TOP
KERNEL_STACK_START:
    .space 0x10000 // 16 KiB of stack should be enough
KERNEL_STACK_TOP:

FIQ_STACK_START:
    .space 1024 // 1 KiB of stack for the FIQ handlers
FIQ_STACK_TOP:

IRQ_STACK_START:
    .space 1024 // 1 KiB of stack for the FIQ handlers
IRQ_STACK_TOP:

ABT_STACK_START:
    .space 1024 // 1 KiB of stack for abort modes
ABT_STACK_TOP:

// TODO: We are not quite sure what this processor mode is and why it needs a stack,
// TODO: but the previous people gave it as stack as well so it seems useful...
MON_STACK_START:
    .space 1024 // 1 KiB of stack for monitor modes
MON_STACK_TOP:

UND_STACK_START:
    .space 1024 // 1 KiB of stack for undefined modes
UND_STACK_TOP:

SYS_STACK_START:
    .space 1024 // 1 KiB of stack for undefined modes
SYS_STACK_TOP:
