.global _Reset
bal _Reset

_Reset:
    LDR sp, =stack_top
    MOV R0, R2
    BL start
    WFI

