# Device Tree Files for QEMU

In this directory, there are a number of DTB files we use when running the kernel in QEMU. 

## What is a DTB file?
In an ARM board, most peripherals are use via memory-mapped IO, which means there is no standard way to poll what kind of peripherals are present in the system and where in memory to find them. Additionally, there is no way to find out how much memory is present in the system. A Device Tree is the standardized way in which the bootloader communicates to the kernel how the system looks like, including the type of cpu, the amount of memory available, what peripherals can be found and any other details we need to for our kernel to interact with the current board correctly.

A **DTS** file is the human-readable representation of a device tree, while a **DTB** is a binary version, more suitable for parsing, and usually what you feed into the OS.

## How does the kernel obtain the DTB file?
One of the responsibilities of a board's bootloader is to load the board's DTB file and pass it to the kernel. Thus on kernel startup, we can expect a pointer to the DTB to be in the `r2` register.

In QEMU however, there is no provision made for a DTB to be passed to our program. Thus we must work around it, and build the DTB into the kernel, and use this built-in DTB instead.

## Why use a dumped DTB file?
Certain bootloaders make changes to the board's standard DTB file before passing it to the kernel. For example, the Raspberry Pi Zero's default DTB does not have the memory property filled in. Thus, if we weren't using a dumped DTB, we would not be getting the correct memory value.

## Further Reading
https://devicetree-specification.readthedocs.io/en/v0.3/
https://devicetree-specification.readthedocs.io/en/v0.3/flattened-format.html
https://elinux.org/Device_Tree_Usage
