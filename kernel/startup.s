bal _Reset

_Reset:
 CPS #0x13
 LDR sp, =svc_stack
 CPS #0x17
 LDR sp, =abort_stack
 MOV R0, R2
 BL start
 WFI