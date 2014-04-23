.text
	.code 32

.equ stack_size, 256
.equ stack_base, 0x20FFF8

.equ Mode_USR, 0x10
.equ Mode_FIQ, 0x11
.equ Mode_IRQ, 0x12
.equ Mode_SVC, 0x13
.equ Mode_MON, 0x16
.equ Mode_ABT, 0x17
.equ Mode_UND, 0x1B
.equ Mode_SYS, 0x1F

	.global	vector_table_start
	.global vector_table_end

.global _Reset
	
	/* exception vector table */
	/* will be copied over to 0x00 upon reset */
	vector_table_start:
		LDR	PC, reset_handler_addr
		LDR	PC, undef_instruction_handler_addr
		LDR	PC, software_interrupt_handler_addr
		LDR	PC, prefetch_abort_handler_addr
		LDR	PC, data_abort_handler_addr
		LDR	PC, reserved_handler_addr
		LDR	PC, irq_handler_addr
		LDR	PC, fiq_handler_addr

		reset_handler_addr:			.word	_Reset
		undef_instruction_handler_addr:		.word	undef_instruction_handler
		software_interrupt_handler_addr:	.word	software_interrupt_handler
		prefetch_abort_handler_addr:		.word	prefetch_abort_handler
		data_abort_handler_addr:		.word	data_abort_handler
		reserved_handler_addr:			.word	reserved_handler
		irq_handler_addr:			.word	irq_handler
		fiq_handler_addr:			.word	fiq_handler
	vector_table_end:

bal _Reset

_Reset:
 LDR sp, =stack_top
	  MOV R0, R2
	   BL start
	   WFI
