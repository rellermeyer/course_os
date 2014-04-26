ENTRY(_Reset)
SECTIONS
{
 . = 0xf0010000;
 .startup : AT(0x10000) { startup.o(.text) mmap.o(.text)}
 .text : { *(.text) }
 .data : { *(.data) }
 .bss : { *(.bss COMMON) }
 . = ALIGN(8);
 . = . + 0x1000; 
 stack_top = . - 0xf0000000;

}
