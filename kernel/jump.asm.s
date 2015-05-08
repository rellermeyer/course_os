/*
 *
 * Jump (ASM)
 *
 * Complimentary assembly functions for jumping between lines of code
 *
 * contact: Mathew Kurian <bluejamesbond@gmail.com>
 *	        Lane Kolbly <lane@rscheme.org>
 * TODO
 * - change BX to MOV so as to not mess with the link register and
 *  status register information
 * - store the cpsr perhaps?
 */

.global jmp_set
.global jmp_goto

# Receives a pointer to a buffer in r0
jmp_set:
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
	mov r0, #0	// set the r0 to zero for the first time return
	bx lr

jmp_goto:
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
	mov r0, r1	// set r1 from argument
	bx lr  // jump back to the link register address
