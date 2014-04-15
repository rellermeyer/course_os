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
	.file	"start.c"
	.data
	.align	2
	.type	first_level_pt, %object
	.size	first_level_pt, 4
first_level_pt:
	.word	134168576
	.align	2
	.type	v_first_level_pt, %object
	.size	v_first_level_pt, 4
v_first_level_pt:
	.word	-268492800
	.section	.rodata
	.align	2
.LC0:
	.ascii	"arguments: \000"
	.align	2
.LC1:
	.ascii	"\012\000"
	.align	2
.LC2:
	.ascii	"CourseOS!\012\000"
	.align	2
.LC3:
	.ascii	"Virtual Memory!!!\012\000"
	.text
	.align	2
	.global	start
	.type	start, %function
start:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #16
	str	r0, [fp, #-16]
	ldr	r0, .L4
	bl	print_uart0
	ldr	r0, [fp, #-16]
	bl	read_cmdline_tag
	mov	r3, r0
	mov	r0, r3
	bl	print_uart0
	ldr	r0, .L4+4
	bl	print_uart0
	ldr	r0, .L4+8
	bl	print_uart0
	bl	mmap
	ldr	r0, .L4+12
	bl	print_vuart0
	mov	r3, #0
	str	r3, [fp, #-8]
	b	.L2
.L3:
	ldr	r3, .L4+16
	ldr	r2, [r3]
	ldr	r3, [fp, #-8]
	mov	r3, r3, asl #2
	add	r3, r2, r3
	ldr	r2, .L4+20
	ldr	r1, [r2]
	ldr	r2, [fp, #-8]
	mov	r2, r2, asl #2
	add	r2, r1, r2
	ldr	r2, [r2]
	str	r2, [r3]
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L2:
	ldr	r3, [fp, #-8]
	cmp	r3, #4096
	blt	.L3
	ldr	r3, .L4+20
	ldr	r3, [r3]
	mov	r2, #0
	str	r2, [r3]
	ldr	r3, .L4+16
	ldr	r3, [r3]
	mov	r2, #0
	str	r2, [r3]
	ldr	r3, .L4+20
	ldr	r3, [r3]
	add	r3, r3, #508
	mov	r2, #0
	str	r2, [r3]
	ldr	r3, .L4+16
	ldr	r3, [r3]
	add	r3, r3, #508
	mov	r2, #0
	str	r2, [r3]
@ 33 "start.c" 1
	eor r0, r0 
	MCR p15, 0, r0, c8, c7, 0 
	
@ 0 "" 2
@ 38 "start.c" 1
	.include "stacks.s"
@ 0 "" 2
@ 40 "start.c" 1
	wfi
@ 0 "" 2
	sub	sp, fp, #4
	@ sp needed
	ldmfd	sp!, {fp, pc}
.L5:
	.align	2
.L4:
	.word	.LC0
	.word	.LC1
	.word	.LC2
	.word	.LC3
	.word	v_first_level_pt
	.word	first_level_pt
	.size	start, .-start
	.ident	"GCC: (GNU) 4.8.1"
