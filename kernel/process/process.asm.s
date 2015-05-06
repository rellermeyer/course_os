.global process_set_umode_sp
.global process_save_state
.global process_load_state

process_set_umode_sp:
	// Switch to system mode (same SP as user mode)
	mov r0,lr
	mov r1,sp
	msr cpsr, #0x1F
	mov lr,r0
	mov sp,r1
	bx lr

process_save_state:
	str r2, [r0, #8]
	str r3, [r0, #12]
	str r4, [r0, #16]
	str r5, [r0, #20]
	str r6, [r0, #24]
	str r7, [r0, #28]
	str r8, [r0, #32]
	str r9, [r0, #36]
	str r10, [r0, #40]
	str r11, [r0, #44]
	str r12, [r0, #48]
	str r13, [r0, #52]
	str r14, [r0, #56]
	str r15, [r0, #60]
	mov r0, #0
	bx lr

process_load_state:
	ldr r1, [r0, #4]
	ldr r2, [r0, #8]
	ldr r3, [r0, #12]
	ldr r4, [r0, #16]
	ldr r5, [r0, #20]
	ldr r6, [r0, #24]
	ldr r7, [r0, #28]
	ldr r8, [r0, #32]
	ldr r9, [r0, #36]
	ldr r10, [r0, #40]
	ldr r11, [r0, #44]
	ldr r12, [r0, #48]
	ldr r13, [r0, #52]
	ldr r14, [r0, #56]
	ldr r0, [r0, #0]
	msr cpsr, #0x1F
	movs pc, lr
