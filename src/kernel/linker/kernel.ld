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
        */startup.o (.bss)
        */stacks.o (.text)
        */memory.o (.text)
        */memory.o (.data)
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

    .process_exec : AT(ADDR(.process_exec) - __KERNEL_VIRTUAL_OFFSET) {
	*/simple_program.o (.exec_file)
    }

    /*make kernel top megabyte aligned*/
    . = ALIGN(1024 * 1024);
    __KERNEL_TOP = .;
}

/* MEMORY */
/* { */
/*     RAM (rw) : org = 0x47912343, len = 1M */
/* } */
