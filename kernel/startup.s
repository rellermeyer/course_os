.global _Reset

_Reset:
    LDR sp, =stack_top
    MOV R0, R2
    BL start
loop:
    WFI
    B loop
