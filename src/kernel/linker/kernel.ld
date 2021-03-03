ENTRY(_Reset)

__KERNEL_VIRTUAL_OFFSET = 0x80000000;
__BOOT_ADDRESS = 0x8000;
__USER_VIRTUAL_OFFSET = 0x500000;

/* TODO what to do here
    need to put a "shared library" somewhere in userspace
    in order to drop to user space
*/

MEMORY
{
    vas : ORIGIN = __USER_VIRTUAL_OFFSET, LENGTH = 0x2000 /* TODO lenght ? */
}
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

    /*. ALIGN(0x1000);*/
    . += __USER_VIRTUAL_OFFSET;
    /* . = ALIGN(4096); */
    user_start = .;
    .text.user : { */dispatcher.o (.text) } > vas
    user_end = .;

    . += __KERNEL_VIRTUAL_OFFSET - __USER_VIRTUAL_OFFSET;

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
