.equ stack_size, 256
.equ stack_base, 0x20FFF8

.equ Mode_USR, 0x10
.equ Mode_FIQ, 0x11
.equ Mode_IRQ, 0x12
.equ Mode_SVC, 0x13
.equ Mode_MON, 0x16
.equ Mode_ABT, 0x17
.equ Mode_UND, 0x1B
.equ Mode_SYS, 0x1F

bal _Reset

_Reset:
 CPS #0x13
 LDR sp, =svc_stack
 CPS #0x17
 LDR sp, =abort_stack
 MOV R0, R2
 BL start
 WFI

