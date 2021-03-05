ENTRY(_Reset)

__KERNEL_VIRTUAL_OFFSET = 0x80000000;
__BOOT_ADDRESS = 0x8000;

SECTIONS
{
    . = __BOOT_ADDRESS;
    __BOOT_START = .;
    .boot : {
        */startup.o (.text)
        */startup.o (.data)
        */stacks.o (.text)
        */startup.o (.bss)
    }
    __BOOT_END = .;
    . += __KERNEL_VIRTUAL_OFFSET;

    __KERNEL_BASE = .;
    .text : AT(ADDR(.text) - __KERNEL_VIRTUAL_OFFSET) {
        *(EXCLUDE_FILE (*/startup.o */stacks.o) .text)
        *(.rodata*)
    }

    .data : AT(ADDR(.data) - __KERNEL_VIRTUAL_OFFSET){
        *(EXCLUDE_FILE (*/startup.o) .data)
     }

    .bss : AT(ADDR(.bss) - __KERNEL_VIRTUAL_OFFSET) {
        *(EXCLUDE_FILE (*/startup.o) COMMON)
        *(EXCLUDE_FILE (*/startup.o) .bss)
    }

    /*make kernel top megabyte aligned*/
    . = ALIGN(1024 * 1024);
    __KERNEL_TOP = .;
}
