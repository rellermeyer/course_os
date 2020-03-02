ENTRY(_Reset)
SECTIONS
{
    . = 0x8000;
    P_KERNBASE = .;
    .startup . : { build/common/startup.o(.text) }
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss COMMON) }
    P_KERNTOP = .;
    . = ALIGN(8);
    . = . + 0x1000;
    stack_top = .;
}

