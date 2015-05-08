ENTRY(_Reset)
SECTIONS
{
	. = 0x8000;
	.startup . : { startup.o(.text) }
	.text : { *(.text) }
 	.data : { *(.data) }
	.bss : { *(.bss COMMON) }
 	. = ALIGN(8);
	. = . + 0x1000; 
	stack_top = .;
}