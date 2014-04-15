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
	.file	"pmap.c"
	.data
	.align	2
	.type	v_first_level_pt, %object
	.size	v_first_level_pt, 4
v_first_level_pt:
	.word	-268492800
	.align	2
	.type	first_level_pt, %object
	.size	first_level_pt, 4
first_level_pt:
	.word	134168576
	.global	next_frame
	.align	2
	.type	next_frame, %object
	.size	next_frame, 4
next_frame:
	.word	1048576
	.text
	.align	2
	.global	get_frame
	.type	get_frame, %function
get_frame:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #20
	str	r0, [fp, #-16]
	ldr	r3, .L5
	ldr	r3, [r3]
	str	r3, [fp, #-8]
	ldr	r3, [fp, #-16]
	mov	r2, r3, asl #12
	ldr	r3, .L5
	ldr	r3, [r3]
	add	r3, r2, r3
	cmp	r3, #128974848
	bcs	.L2
	ldr	r3, [fp, #-16]
	mov	r2, r3, asl #12
	ldr	r3, .L5
	ldr	r3, [r3]
	add	r2, r2, r3
	ldr	r3, .L5
	str	r2, [r3]
	b	.L3
.L2:
	ldr	r3, .L5
	mov	r2, #0
	str	r2, [r3]
.L3:
	ldr	r3, .L5
	ldr	r3, [r3]
	mov	r0, r3
	sub	sp, fp, #0
	@ sp needed
	ldr	fp, [sp], #4
	bx	lr
.L6:
	.align	2
.L5:
	.word	next_frame
	.size	get_frame, .-get_frame
	.ident	"GCC: (GNU) 4.8.1"
