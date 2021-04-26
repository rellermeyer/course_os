ENTRY(_Reset)

__KERNEL_VIRTUAL_OFFSET = 0x80000000;
__BOOT_ADDRESS = 0x8000;

SECTIONS
{
    . = __BOOT_ADDRESS;

    __BOOT_START = .;
    .boot : {
        */src_common_startup.s.o (.text)
        */src_common_startup.s.o (.data)
        */src_common_startup.s.o (.bss)
    __DTB_START = .;
        */dtb.o (.data)
        */src_common_stacks.s.o (.text)
        */src_common_memory.s.o (.text)
        */src_common_memory.s.o (.data)
    }
    __BOOT_END = .;


    . += __KERNEL_VIRTUAL_OFFSET;

    __KERNEL_BASE = .;
    .text : AT(ADDR(.text) - __KERNEL_VIRTUAL_OFFSET) {
        *(EXCLUDE_FILE (*/src_common_startup.s.o */src_common_stacks.s.o) .text)
        *(.rodata*)
    }

    .data : AT(ADDR(.data) - __KERNEL_VIRTUAL_OFFSET){
        *(EXCLUDE_FILE (*/src_common_startup.s.o) .data)
        __PROCESS_START = .;
        */process_wrapper.mo (.exec_file)
     }

    .bss : AT(ADDR(.bss) - __KERNEL_VIRTUAL_OFFSET) {
        *(EXCLUDE_FILE (*/src_common_startup.s.o) COMMON)
        *(EXCLUDE_FILE (*/src_common_startup.s.o) .bss)
    }

    /*make kernel top megabyte aligned*/
    . = ALIGN(1024 * 1024);
    __KERNEL_TOP = .;
}
