.global _Reset

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

_Reset:
    // Disable other cores
    mrc p15, #0, r1, c0, c0, #5
    and r1, r1, #3
    cmp r1, #0
    bne loop

    LDR sp, =KERNEL_STACK_TOP // Set the kernel/SVC stack

    // FIQ Stack
    MSR CPSR_c, #Mode_FIQ // Switch to FIQ Mode
    LDR sp, =FIQ_STACK_TOP // Set the FIQ stack pointer

    // IRQ Stack
    MSR CPSR_c, #Mode_IRQ
    LDR sp, =IRQ_STACK_TOP

    // MON Stack
    MSR CPSR_c, #Mode_MON
    LDR sp, =MON_STACK_TOP

    // ABT Stack
    MSR CPSR_c, #Mode_ABT
    LDR sp, =ABT_STACK_TOP

    // UND Stack
    MSR CPSR_c, #Mode_UND
    LDR sp, =UND_STACK_TOP

    // SYS Stack
    MSR CPSR_c, #Mode_SYS
    LDR sp, =SYS_STACK_TOP

    // Switch back SVC/Kernel
    MSR CPSR_c, #Mode_SVC
    ADD fp, sp, #0 // ???

    MOV R0, R2 // boot args in R0
    BL start
loop:
    WFI
    B loop

.section .bss
//.global KERNEL_STACK_TOP, KERNEL_STACK_BASE, FIQ_STACK_START, FIQ_STACK_TOP, IRQ_STACK_START, IRQ_STACK_TOP

KERNEL_STACK_START:
    .space 4096 // 4 KiB of stack should be enough
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

