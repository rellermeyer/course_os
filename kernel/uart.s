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
	.file	"uart.c"
	.global	UART
	.section	.rodata
	.align	2
	.type	UART, %object
	.size	UART, 4
UART:
	.word	270471168
	.global	UARTV
	.align	2
	.type	UARTV, %object
	.size	UARTV, 4
UARTV:
	.word	-2145447936
	.text
	.align	2
	.global	print_uart0
	.type	print_uart0, %function
print_uart0:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #12
	str	r0, [fp, #-8]
	b	.L2
.L3:
	ldr	r3, .L4
	ldr	r2, [fp, #-8]
	ldrb	r2, [r2]	@ zero_extendqisi2
	str	r2, [r3]
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L2:
	ldr	r3, [fp, #-8]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #0
	bne	.L3
	sub	sp, fp, #0
	@ sp needed
	ldr	fp, [sp], #4
	bx	lr
.L5:
	.align	2
.L4:
	.word	270471168
	.size	print_uart0, .-print_uart0
	.align	2
	.global	print_vuart0
	.type	print_vuart0, %function
print_vuart0:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #12
	str	r0, [fp, #-8]
	b	.L7
.L8:
	ldr	r3, .L9
	ldr	r2, [fp, #-8]
	ldrb	r2, [r2]	@ zero_extendqisi2
	str	r2, [r3]
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L7:
	ldr	r3, [fp, #-8]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #0
	bne	.L8
	sub	sp, fp, #0
	@ sp needed
	ldr	fp, [sp], #4
	bx	lr
.L10:
	.align	2
.L9:
	.word	-2145447936
	.size	print_vuart0, .-print_vuart0
	.align	2
	.global	print_char_uart0
	.type	print_char_uart0, %function
print_char_uart0:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #12
	mov	r3, r0
	strb	r3, [fp, #-5]
	ldr	r3, .L12
	ldrb	r2, [fp, #-5]	@ zero_extendqisi2
	str	r2, [r3]
	sub	sp, fp, #0
	@ sp needed
	ldr	fp, [sp], #4
	bx	lr
.L13:
	.align	2
.L12:
	.word	-2145447936
	.size	print_char_uart0, .-print_char_uart0
	.ident	"GCC: (GNU) 4.8.1"
