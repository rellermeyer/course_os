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
.global	_Reset

/* exception vector table */
/* will be copied over to 0x00 upon reset */
vector_table_start:
	b	_Reset
	b	undef_instruction_handler
	b	software_interrupt_handler
	b	prefetch_abort_handler
	b	data_abort_handler
	b	.
	b	irq_handler
	b	fiq_handler
vector_table_end:

_Reset:

/* ensure we're in supervisor mode w/ irq/fiq disabled */
MSR	cpsr_c, #Mode_SVC
LDR	sp, =stack_top
BL 	start

WFI
