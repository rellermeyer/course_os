bal _Reset

_Reset:

 CPS #0x1f
 LDR sp, =system_stack

 CPS #0x12
 LDR sp, =irq_stack

 CPS #0x17
 LDR sp, =abort_stack

 CPS #0x13
 LDR sp, =svc_stack

 MOV R0, R2
 BL initvm
 WFI