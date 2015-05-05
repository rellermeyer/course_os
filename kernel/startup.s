.equ Mode_USR,            0x10
.equ Mode_FIQ,            0x11
.equ Mode_IRQ,            0x12
.equ Mode_SVC,            0x13
.equ Mode_ABT,            0x17
.equ Mode_UND,            0x1B
.equ Mode_SYS,            0x1F

.equ I_Bit,               0x80            @ when I bit is set, IRQ is disabled
.equ F_Bit,               0x40            @ when F bit is set, FIQ is disabled

.equ UND_Stack_Size,     0x00000000
.equ SVC_Stack_Size,     0x00000200
.equ ABT_Stack_Size,     0x00000000
.equ FIQ_Stack_Size,     0x00000000
.equ IRQ_Stack_Size,     0x00000100
.equ USR_Stack_Size,     0x00000000

.equ Stack_Size,        (UND_Stack_Size + SVC_Stack_Size + ABT_Stack_Size + FIQ_Stack_Size + IRQ_Stack_Size + USR_Stack_Size)

.global _Reset
bal _Reset

_Reset:
    LDR sp, =stack_top

@  Enter Undefined Instruction Mode and set its Stack Pointer
    MSR     CPSR_c, #Mode_UND | I_Bit | F_Bit
    MOV     SP, R0
    SUB     R0, R0, #UND_Stack_Size

@  Enter Abort Mode and set its Stack Pointer
    MSR     CPSR_c, #Mode_ABT | I_Bit | F_Bit
    MOV     SP, R0
    SUB     R0, R0, #ABT_Stack_Size

@  Enter FIQ Mode and set its Stack Pointer
    MSR     CPSR_c, #Mode_FIQ | I_Bit | F_Bit
    MOV     SP, R0
    SUB     R0, R0, #FIQ_Stack_Size

@  Enter IRQ Mode and set its Stack Pointer
    MSR     CPSR_c, #Mode_IRQ | I_Bit | F_Bit
    MOV     SP, R0
    SUB     R0, R0, #IRQ_Stack_Size

@  Enter Supervisor Mode and set its Stack Pointer
    MSR     CPSR_c, #Mode_SVC | I_Bit | F_Bit
    MOV     SP, R0
    SUB     SL, SP, #SVC_Stack_Size
@
@  Enter User Mode and set its Stack Pointer
    MSR     CPSR_c, #Mode_USR
    MOV     SP, R0
    SUB     SL, SP, #USR_Stack_Size

    MOV R0, R2
    BL start
    WFI

