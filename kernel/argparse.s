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
	.file	"argparse.c"
	.text
	.align	2
	.global	parse_arguments
	.type	parse_arguments, %function
parse_arguments:
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #24
	str	r0, [fp, #-24]
	str	r1, [fp, #-28]
	ldr	r3, [fp, #-28]
	str	r3, [fp, #-8]
	mov	r3, #0
	str	r3, [fp, #-12]
	b	.L2
.L3:
	mov	r3, #0
	str	r3, [fp, #-16]
	ldr	r0, [fp, #-8]
	bl	analyze_args
	mov	r3, r0
	ldr	r2, [fp, #-16]
	add	r3, r2, r3
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-16]
	mov	r3, r3, asl #2
	ldr	r2, [fp, #-8]
	add	r3, r2, r3
	str	r3, [fp, #-8]
	ldr	r2, [fp, #-12]
	ldr	r3, [fp, #-16]
	add	r3, r2, r3
	str	r3, [fp, #-12]
.L2:
	ldr	r2, [fp, #-12]
	ldr	r3, [fp, #-24]
	cmp	r2, r3
	blt	.L3
	sub	sp, fp, #4
	@ sp needed
	ldmfd	sp!, {fp, pc}
	.size	parse_arguments, .-parse_arguments
	.align	2
	.global	analyze_args
	.type	analyze_args, %function
analyze_args:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #20
	str	r0, [fp, #-16]
	mov	r3, #0
	str	r3, [fp, #-8]
	mov	r3, #1
	mov	r0, r3
	sub	sp, fp, #0
	@ sp needed
	ldr	fp, [sp], #4
	bx	lr
	.size	analyze_args, .-analyze_args
	.align	2
	.global	read_cmdline_tag
	.type	read_cmdline_tag, %function
read_cmdline_tag:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #36
	str	r0, [fp, #-32]
	ldr	r3, [fp, #-32]
	str	r3, [fp, #-8]
	ldr	r3, .L10
	str	r3, [fp, #-20]
	mov	r3, #0
	str	r3, [fp, #-12]
	mov	r3, #0
	str	r3, [fp, #-16]
	b	.L7
.L8:
	ldr	r3, [fp, #-12]
	mov	r3, r3, asl #2
	ldr	r2, [fp, #-8]
	add	r3, r2, r3
	str	r3, [fp, #-8]
	ldr	r3, [fp, #-8]
	ldr	r3, [r3]
	str	r3, [fp, #-12]
	ldr	r3, [fp, #-8]
	add	r3, r3, #4
	str	r3, [fp, #-8]
	ldr	r3, [fp, #-8]
	ldr	r3, [r3]
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-8]
	sub	r3, r3, #4
	str	r3, [fp, #-8]
.L7:
	ldr	r2, [fp, #-16]
	ldr	r3, [fp, #-20]
	cmp	r2, r3
	bne	.L8
	ldr	r3, [fp, #-8]
	add	r3, r3, #8
	str	r3, [fp, #-8]
	ldr	r3, [fp, #-8]
	str	r3, [fp, #-24]
	ldr	r3, [fp, #-24]
	mov	r0, r3
	sub	sp, fp, #0
	@ sp needed
	ldr	fp, [sp], #4
	bx	lr
.L11:
	.align	2
.L10:
	.word	1413545993
	.size	read_cmdline_tag, .-read_cmdline_tag
	.ident	"GCC: (GNU) 4.8.1"
