/*
 *
 * Hardware Handlers (ASM)
 *
 * Complimentary functions to the hw_handlers.c. Refer to the use cases
 * of these functions to learn move about them
 *
 * contact: Mathew Kurian <bluejamesbond@gmail.com>
 *
 */

.global hw_save_system_mode_banked_regs

// r0 has pointer to jmp_buffer
hw_save_system_mode_banked_regs:
	mrs r2, cpsr  // backup cpsr
	msr cpsr, #0x1F
	str r8, [r0, #32]
	str r9, [r0, #36]
	str r10, [r0, #40]
	str r11, [r0, #44]
	str r12, [r0, #48]
	str r13, [r0, #52]
	str r14, [r0, #56]
	str r15, [r0, #60]
	msr cpsr, r2
	bx lr
