	.cpu arm1176jzf-s
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"klibc.c"
	.data
	.align	2
	.type	lower_case_digits, %object
	.size	lower_case_digits, 16
lower_case_digits:
	.ascii	"0123456789abcdef"
	.align	2
	.type	upper_case_digits, %object
	.size	upper_case_digits, 16
upper_case_digits:
	.ascii	"0123456789ABCDEF"
	.text
	.align	2
	.global	os_memcmp
	.type	os_memcmp, %function
os_memcmp:
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #28
	str	r0, [fp, #-16]
	str	r1, [fp, #-20]
	str	r2, [fp, #-24]
	ldr	r3, [fp, #-16]
	str	r3, [fp, #-8]
	ldr	r3, [fp, #-20]
	str	r3, [fp, #-12]
	b	.L2
.L4:
	ldr	r3, [fp, #-24]
	sub	r3, r3, #1
	str	r3, [fp, #-24]
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
	ldr	r3, [fp, #-12]
	add	r3, r3, #1
	str	r3, [fp, #-12]
.L2:
	ldr	r3, [fp, #-24]
	cmp	r3, #0
	beq	.L3
	ldr	r3, [fp, #-8]
	ldrb	r2, [r3]	@ zero_extendqisi2
	ldr	r3, [fp, #-12]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r2, r3
	beq	.L4
.L3:
	ldr	r3, [fp, #-24]
	cmp	r3, #0
	beq	.L5
	ldr	r3, [fp, #-8]
	ldrb	r3, [r3]	@ zero_extendqisi2
	mov	r2, r3
	ldr	r3, [fp, #-12]
	ldrb	r3, [r3]	@ zero_extendqisi2
	rsb	r3, r3, r2
	b	.L6
.L5:
	mov	r3, #0
.L6:
	mov	r0, r3
	sub	sp, fp, #0
	@ sp needed
	ldr	fp, [sp], #4
	bx	lr
	.size	os_memcmp, .-os_memcmp
	.align	2
	.global	os_strcmp
	.type	os_strcmp, %function
os_strcmp:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #12
	str	r0, [fp, #-8]
	str	r1, [fp, #-12]
	b	.L9
.L11:
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
	ldr	r3, [fp, #-12]
	add	r3, r3, #1
	str	r3, [fp, #-12]
.L9:
	ldr	r3, [fp, #-8]
	ldrb	r2, [r3]	@ zero_extendqisi2
	ldr	r3, [fp, #-12]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r2, r3
	bne	.L10
	ldr	r3, [fp, #-8]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #0
	bne	.L11
.L10:
	ldr	r3, [fp, #-8]
	ldrb	r3, [r3]	@ zero_extendqisi2
	mov	r2, r3
	ldr	r3, [fp, #-12]
	ldrb	r3, [r3]	@ zero_extendqisi2
	rsb	r3, r3, r2
	mov	r0, r3
	sub	sp, fp, #0
	@ sp needed
	ldr	fp, [sp], #4
	bx	lr
	.size	os_strcmp, .-os_strcmp
	.align	2
	.global	print_hex
	.type	print_hex, %function
print_hex:
	@ args = 0, pretend = 0, frame = 128
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #128
	str	r0, [fp, #-128]
	str	r1, [fp, #-132]
	ldr	r3, [fp, #-128]
	str	r3, [fp, #-8]
	mov	r3, #0
	str	r3, [fp, #-12]
	mov	r3, #15
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-8]
	cmp	r3, #0
	bne	.L14
	mov	r0, #48
	bl	print_char_uart0
	b	.L15
.L14:
	b	.L15
.L19:
	ldr	r2, [fp, #-8]
	ldr	r3, [fp, #-16]
	and	r3, r2, r3
	str	r3, [fp, #-20]
	ldr	r3, [fp, #-132]
	cmp	r3, #1
	bne	.L16
	ldr	r2, .L22
	ldr	r3, [fp, #-20]
	add	r3, r2, r3
	ldrb	r2, [r3]	@ zero_extendqisi2
	mvn	r3, #115
	ldr	r1, [fp, #-12]
	sub	r0, fp, #4
	add	r1, r0, r1
	add	r3, r1, r3
	strb	r2, [r3]
	b	.L17
.L16:
	ldr	r2, .L22+4
	ldr	r3, [fp, #-20]
	add	r3, r2, r3
	ldrb	r2, [r3]	@ zero_extendqisi2
	mvn	r3, #115
	ldr	r1, [fp, #-12]
	sub	r0, fp, #4
	add	r1, r0, r1
	add	r3, r1, r3
	strb	r2, [r3]
.L17:
	ldr	r3, [fp, #-8]
	mov	r3, r3, asr #4
	str	r3, [fp, #-8]
	ldr	r3, [fp, #-12]
	add	r3, r3, #1
	str	r3, [fp, #-12]
.L15:
	ldr	r3, [fp, #-8]
	cmp	r3, #0
	beq	.L18
	ldr	r3, [fp, #-12]
	cmp	r3, #7
	ble	.L19
.L18:
	b	.L20
.L21:
	ldr	r3, [fp, #-12]
	sub	r2, r3, #1
	mvn	r3, #115
	sub	r1, fp, #4
	add	r2, r1, r2
	add	r3, r2, r3
	ldrb	r3, [r3]	@ zero_extendqisi2
	mov	r0, r3
	bl	print_char_uart0
	ldr	r3, [fp, #-12]
	sub	r3, r3, #1
	str	r3, [fp, #-12]
.L20:
	ldr	r3, [fp, #-12]
	cmp	r3, #0
	bgt	.L21
	sub	sp, fp, #4
	@ sp needed
	ldmfd	sp!, {fp, pc}
.L23:
	.align	2
.L22:
	.word	upper_case_digits
	.word	lower_case_digits
	.size	print_hex, .-print_hex
	.section	.rodata
	.align	2
.LC0:
	.ascii	"0\000"
	.align	2
.LC1:
	.ascii	"-\000"
	.text
	.align	2
	.global	print_dec
	.type	print_dec, %function
print_dec:
	@ args = 0, pretend = 0, frame = 120
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #120
	str	r0, [fp, #-120]
	ldr	r3, [fp, #-120]
	str	r3, [fp, #-8]
	mov	r3, #0
	str	r3, [fp, #-12]
	ldr	r3, [fp, #-8]
	cmp	r3, #0
	bne	.L25
	ldr	r0, .L31
	bl	print_uart0
	b	.L26
.L25:
	ldr	r3, [fp, #-8]
	cmp	r3, #0
	bge	.L26
	ldr	r0, .L31+4
	bl	print_uart0
	ldr	r3, [fp, #-8]
	rsb	r3, r3, #0
	str	r3, [fp, #-8]
	b	.L27
.L26:
	b	.L27
.L28:
	ldr	r2, [fp, #-8]
	ldr	r3, .L31+8
	smull	r0, r3, r3, r2
	mov	r1, r3, asr #2
	mov	r3, r2, asr #31
	rsb	r1, r3, r1
	mov	r3, r1
	mov	r3, r3, asl #2
	add	r3, r3, r1
	mov	r3, r3, asl #1
	rsb	r3, r3, r2
	str	r3, [fp, #-16]
	ldr	r2, .L31+12
	ldr	r3, [fp, #-16]
	add	r3, r2, r3
	ldrb	r2, [r3]	@ zero_extendqisi2
	mvn	r3, #111
	ldr	r1, [fp, #-12]
	sub	r0, fp, #4
	add	r1, r0, r1
	add	r3, r1, r3
	strb	r2, [r3]
	ldr	r3, [fp, #-8]
	ldr	r2, .L31+8
	smull	r1, r2, r2, r3
	mov	r2, r2, asr #2
	mov	r3, r3, asr #31
	rsb	r3, r3, r2
	str	r3, [fp, #-8]
	ldr	r3, [fp, #-12]
	add	r3, r3, #1
	str	r3, [fp, #-12]
.L27:
	ldr	r3, [fp, #-8]
	cmp	r3, #0
	bne	.L28
	b	.L29
.L30:
	ldr	r3, [fp, #-12]
	sub	r2, r3, #1
	mvn	r3, #111
	sub	r0, fp, #4
	add	r2, r0, r2
	add	r3, r2, r3
	ldrb	r3, [r3]	@ zero_extendqisi2
	mov	r0, r3
	bl	print_char_uart0
	ldr	r3, [fp, #-12]
	sub	r3, r3, #1
	str	r3, [fp, #-12]
.L29:
	ldr	r3, [fp, #-12]
	cmp	r3, #0
	bgt	.L30
	mov	r0, r3
	sub	sp, fp, #4
	@ sp needed
	ldmfd	sp!, {fp, pc}
.L32:
	.align	2
.L31:
	.word	.LC0
	.word	.LC1
	.word	1717986919
	.word	upper_case_digits
	.size	print_dec, .-print_dec
	.section	.rodata
	.align	2
.LC2:
	.ascii	"%\000"
	.text
	.align	2
	.global	os_printf
	.type	os_printf, %function
os_printf:
	@ args = 4, pretend = 16, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 1
	stmfd	sp!, {r0, r1, r2, r3}
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #16
	add	r3, fp, #8
	str	r3, [fp, #-16]
	mov	r3, #0
	str	r3, [fp, #-8]
	b	.L34
.L44:
	ldr	r3, [fp, #4]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #37
	bne	.L35
	ldr	r3, [fp, #4]
	add	r3, r3, #1
	str	r3, [fp, #4]
	ldr	r3, [fp, #4]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #99
	beq	.L37
	cmp	r3, #99
	bgt	.L38
	cmp	r3, #37
	beq	.L39
	cmp	r3, #88
	beq	.L40
	b	.L43
.L38:
	cmp	r3, #100
	beq	.L41
	cmp	r3, #120
	beq	.L42
	b	.L43
.L40:
	ldr	r3, [fp, #-16]
	add	r2, r3, #4
	str	r2, [fp, #-16]
	ldr	r3, [r3]
	str	r3, [fp, #-12]
	ldr	r0, [fp, #-12]
	mov	r1, #1
	bl	print_hex
	b	.L36
.L42:
	ldr	r3, [fp, #-16]
	add	r2, r3, #4
	str	r2, [fp, #-16]
	ldr	r3, [r3]
	str	r3, [fp, #-12]
	ldr	r0, [fp, #-12]
	mov	r1, #0
	bl	print_hex
	b	.L36
.L41:
	ldr	r3, [fp, #-16]
	add	r2, r3, #4
	str	r2, [fp, #-16]
	ldr	r3, [r3]
	str	r3, [fp, #-12]
	ldr	r0, [fp, #-12]
	bl	print_dec
	b	.L36
.L37:
	ldr	r3, [fp, #-16]
	add	r2, r3, #4
	str	r2, [fp, #-16]
	ldr	r3, [r3]
	str	r3, [fp, #-12]
	ldr	r0, [fp, #-12]
	bl	print_char_uart0
	b	.L36
.L39:
	ldr	r0, .L45
	bl	print_uart0
	mov	r0, r0	@ nop
.L36:
	b	.L43
.L35:
	ldr	r3, [fp, #4]
	ldrb	r3, [r3]	@ zero_extendqisi2
	mov	r0, r3
	bl	print_char_uart0
.L43:
	ldr	r3, [fp, #4]
	add	r3, r3, #1
	str	r3, [fp, #4]
.L34:
	ldr	r3, [fp, #4]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #0
	bne	.L44
	mov	r0, r3
	sub	sp, fp, #4
	@ sp needed
	ldmfd	sp!, {fp, lr}
	add	sp, sp, #16
	bx	lr
.L46:
	.align	2
.L45:
	.word	.LC2
	.size	os_printf, .-os_printf
	.ident	"GCC: (GNU) 4.8.1"
