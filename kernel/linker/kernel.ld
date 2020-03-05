ENTRY(_Reset)
SECTIONS
{
    . = 0x8000;
    KERNEL_BASE = .;
    .startup . : { build/common/startup.o(.text) }
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss COMMON)}

    /*make kernel top megabyte aligned*/
    . = ALIGN(1024 * 1024);
    KERNEL_TOP = .;
}
