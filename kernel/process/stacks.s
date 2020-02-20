.equ stack_size, 0x1000
.equ stack_base, 0xfffff000

.equ Mode_USR, 0x10
.equ Mode_FIQ, 0x11
.equ Mode_IRQ, 0x12
.equ Mode_SVC, 0x13
.equ Mode_MON, 0x16
.equ Mode_ABT, 0x17
.equ Mode_UND, 0x1B
.equ Mode_SYS, 0x1F

 MOV	R1, R0 // R0 has p_bootargs, which we need to hang onto
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
 ADD 	 fp, sp, #0

 EOR 	 R0, R0
 ADD 	 R0, pc, #0xf0000000
 MOV 	 pc, R0

 EOR 	 R0, R0
 ADD 	 R0, lr, #0xf0000000
 MOV 	 lr, R0
 MOV	R0, R1
