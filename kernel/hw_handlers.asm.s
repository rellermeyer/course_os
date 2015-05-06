.global hw_get_spmode_sp_lr

hw_get_spmode_sp_lr:
	mrs r2, cpsr  // backup cpsr
	msr cpsr, #0x1F
	mov r0,sp
	mov r1,lr
	msr cpsr, r2
	bx lr
