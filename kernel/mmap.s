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
	.file	"mmap.c"
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
	.text
	.align	2
	.global	mmap
	.type	mmap, %function
mmap:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	stmfd	sp!, {r4, r5, fp}
	add	fp, sp, #8
@ 21 "mmap.c" 1
	push {r0-r11}
@ 0 "" 2
@ 23 "mmap.c" 1
	cpsid if
@ 0 "" 2
	mov	r5, #128974848
	mov	r4, #0
	b	.L2
.L3:
	ldr	r3, .L6
	ldr	r2, [r3]
	mov	r3, r4
	mov	r3, r3, asl #2
	add	r3, r2, r3
	orr	r2, r5, #1
	str	r2, [r3]
	add	r5, r5, #1024
	mov	r3, r4
	add	r3, r3, #1
	mov	r4, r3
.L2:
	mov	r3, r4
	cmp	r3, #4096
	blt	.L3
	ldr	r3, .L6
	ldr	r3, [r3]
	add	r3, r3, #508
	ldr	r2, .L6+4
	str	r2, [r3]
	ldr	r3, .L6
	ldr	r3, [r3]
	add	r3, r3, #15296
	add	r3, r3, #60
	ldr	r2, .L6+4
	str	r2, [r3]
	ldr	r3, .L6
	ldr	r3, [r3]
	add	r3, r3, #15296
	add	r3, r3, #44
	ldr	r2, .L6+8
	str	r2, [r3]
	ldr	r3, .L6
	ldr	r3, [r3]
	add	r3, r3, #15296
	add	r3, r3, #48
	ldr	r2, .L6+12
	str	r2, [r3]
	ldr	r3, .L6
	ldr	r3, [r3]
	add	r3, r3, #15296
	add	r3, r3, #52
	ldr	r2, .L6+16
	str	r2, [r3]
	ldr	r3, .L6
	ldr	r3, [r3]
	add	r3, r3, #15296
	add	r3, r3, #56
	ldr	r2, .L6+20
	str	r2, [r3]
	ldr	r3, .L6
	ldr	r3, [r3]
	ldr	r2, .L6+24
	str	r2, [r3]
	ldr	r3, .L6
	ldr	r3, [r3]
	add	r3, r3, #15360
	ldr	r2, .L6+24
	str	r2, [r3]
	ldr	r3, .L6
	ldr	r3, [r3]
	add	r3, r3, #8192
	ldr	r2, .L6+28
	str	r2, [r3]
	ldr	r3, .L6
	ldr	r3, [r3]
	add	r3, r3, #8192
	add	r3, r3, #4
	ldr	r2, .L6+32
	str	r2, [r3]
	ldr	r4, .L6+36
	b	.L4
.L5:
	ldr	r3, .L6
	ldr	r2, [r3]
	mov	r3, r4
	mov	r3, r3, asl #2
	add	r2, r2, r3
	mov	r3, r4
	sub	r3, r3, #1008
	sub	r3, r3, #2
	mov	r3, r3, asl #20
	orr	r3, r3, #1024
	orr	r3, r3, #2
	str	r3, [r2]
	mov	r3, r4
	add	r3, r3, #1
	mov	r4, r3
.L4:
	mov	r3, r4
	mov	r2, r3
	ldr	r3, .L6+40
	cmp	r2, r3
	bls	.L5
	ldr	r3, .L6
	ldr	r3, [r3]
	mov	r4, r3
@ 80 "mmap.c" 1
	mcr p15, 0, r4, c2, c0, 0
@ 0 "" 2
	mov	r3, #1
@ 87 "mmap.c" 1
	mcr p15, 0, r3, c3, c0, 0
@ 0 "" 2
@ 100 "mmap.c" 1
	mrc p15, 0, r4, c1, c0, 0
@ 0 "" 2
	mov	r3, r4
	orr	r3, r3, #12288
	orr	r3, r3, #7
	mov	r4, r3
@ 104 "mmap.c" 1
	mcr p15, 0, r4, c1, c0, 0
@ 0 "" 2
@ 110 "mmap.c" 1
	eor r4, r4
@ 0 "" 2
@ 111 "mmap.c" 1
	add r4, pc, #0xf0000000
@ 0 "" 2
@ 114 "mmap.c" 1
	mov pc, r4
@ 0 "" 2
@ 117 "mmap.c" 1
	eor r4, r4
@ 0 "" 2
@ 118 "mmap.c" 1
	add r4, lr, #0xf0000000
@ 0 "" 2
@ 119 "mmap.c" 1
	mov lr, r4
@ 0 "" 2
@ 122 "mmap.c" 1
	cpsie fi
@ 0 "" 2
@ 123 "mmap.c" 1
	pop {r0-r11}
@ 0 "" 2
	sub	sp, fp, #8
	@ sp needed
	ldmfd	sp!, {r4, r5, fp}
	bx	lr
.L7:
	.align	2
.L6:
	.word	first_level_pt
	.word	133170178
	.word	128975874
	.word	130024450
	.word	131073026
	.word	132121602
	.word	1026
	.word	268436482
	.word	269485058
	.word	2050
	.word	2801
	.size	mmap, .-mmap
	.ident	"GCC: (GNU) 4.8.1"
