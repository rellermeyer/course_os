.equ stack_size, 0x1000
.equ stack_base, 0xf0000000

.equ Mode_USR, 0x10
.equ Mode_FIQ, 0x11
.equ Mode_IRQ, 0x12
.equ Mode_SVC, 0x13
.equ Mode_MON, 0x16
.equ Mode_ABT, 0x17
.equ Mode_UND, 0x1B
.equ Mode_SYS, 0x1F

 
 LDR     R0, =stack_base
 MSR     CPSR_c, #Mode_FIQ
 MOV     sp, R0
 SUB     R0, R0, #stack_size

 MSR     CPSR_c, #Mode_IRQ
 MOV     sp, R0
 SUB     R0, R0, #stack_size

 MSR     CPSR_c, #Mode_SVC
 MOV     sp, R0
 SUB     R0, R0, #stack_size

 MSR     CPSR_c, #Mode_MON
 MOV     sp, R0
 SUB     R0, R0, #stack_size

 MSR     CPSR_c, #Mode_ABT
 MOV     sp, R0
 SUB     R0, R0, #stack_size

 MSR     CPSR_c, #Mode_UND
 MOV     sp, R0
 SUB     R0, R0, #stack_size

 MSR     CPSR_c, #Mode_SYS
 MOV     sp, R0

 MSR     CPSR_c, #Mode_SVC