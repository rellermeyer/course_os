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
	.file	"main.c"
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
	.section	.rodata
	.align	2
.LC0:
	.ascii	"New supervisor stack at 0xefffd000\012\000"
	.text
	.align	2
	.global	main
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	ldr	r0, .L2
	bl	print_vuart0
@ 11 "main.c" 1
	eor r0, r0 
	MCR p15, 0, r0, c8, c7, 0 
	ldr r0, =0x786 
	push {r0}
@ 0 "" 2
@ 19 "main.c" 1
	wfi
@ 0 "" 2
	ldmfd	sp!, {fp, pc}
.L3:
	.align	2
.L2:
	.word	.LC0
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.1"
