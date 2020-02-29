.global _Reset

_Reset:
    // Disable other cores
    //mrc p15, 0, r5, c0, c0, 5
	//and r5, r5, #3
	//cmp r5, #0
	//bne loop

    LDR sp, =stack_top
    MOV R0, R2
    BL start
loop:
    WFI
    B loop
