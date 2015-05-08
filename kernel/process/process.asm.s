/*
 *
 * Process (ASM)
 *
 * Complimentary functions to the process.c. Refer to the use cases
 * of these functions to learn move about them
 *
 * contact: Lane Kolbly <lane@rscheme> (process_set_umode_sp)
 *			Mathew Kurian <bluejamesbond@gmail.com> (process_load_state)
 *
 */

.global process_set_sysetm_mode_sp
.global process_load_state

process_set_sysetm_mode_sp:
	// Switch to system mode (same SP as user mode)
	mov r0,lr
	mov r1,sp
	msr cpsr, #0x1F
	mov lr,r0
	mov sp,r1
	bx lr

process_load_state:
	ldr r1, [r0, #4]
	ldr r2, [r0, #8]
	ldr r3, [r0, #12]
	ldr r4, [r0, #16]
	ldr r5, [r0, #20]
	ldr r6, [r0, #24]
	ldr r7, [r0, #28]
	mrs r1, cpsr
	bic r1, r1, #0x1f
	orr r1, r1, #0x10
	msr spsr, r1
	ldr r8, [r0, #32]
	ldr r9, [r0, #36]
	ldr r10, [r0, #40]
	ldr r11, [r0, #44]
	ldr r12, [r0, #48]
	ldr r13, [r0, #52]
	ldr r14, [r0, #56]
	ldr r0, [r0, #0]
	movs pc, lr
