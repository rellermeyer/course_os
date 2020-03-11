# Virtual Memory

## Description
The virtual memory implemention consists of the following parts:
* [ASID Allocator](include/asid_allocator.h)
* [Physical Memory Manager (PMM)](include/pmm.h)
* [Virtual Address Space Manager (VAS)](include/vas2.h)
* [Generic Virtual Memory Manager (VM)](include/vm2.h)


The entry point for the virtual memory functionality is the [vm2_start()](vm2.c#L91) method.
It will first map all the physical to the 2 to 3GB region (only the first MiB of this was mapped by [startup.s](../common/startup.s)).
Afterwards it will remove the identity mapping done by that same file. After that is done the kernel is fully higher half.

After mapping the physical memory it will initialize the PMM with the range of memory available, using the [pmm_int()](pmm.c#L17) function.
This function will then use the address space provided to build op all the structures needed for
physical page allocation. A general overview of how this is done and the methods available can be 
found in its [header](include/pmm.h).

And finally after the PMM has been initialized it will clear the pointer to the user pagetable (which was temporarily set).


## Memory map

### Physical

| Address    | Description                                                            |
| ---------- | ---------------------------------------------------------------------- |
| 0x00000000 | Interrupt Vector Table                                                 |
| 0x00004000 | Kernel L1 page table                                                   |
| 0x00080000 | Kernel start                                                           |
| ...        | *Kernel*                                                               |   
| ...        | *Kernel*                                                               |
| KERNEL END | Physical Memory Manager starting point                                 |
| ...        | Physical Memory Manager                                                |
| 0xXXXXXXXX | End of Physical memory (Device dependent, max 1GB)                     |

### Virtual 

| Address    | Description                                                            |
| ---------- | ---------------------------------------------------------------------- |
| 0x00000000 | Virtual Process Address Space                                          |
| 0x80000000 | (start of) remap of physical 0x00000000-0x40000000                     |
| 0x80004000 | Kernel page table                                                      |
| 0x80008000 | Kernel start                                                           |
| ...        | *Kernel*                                                               |   
| ...        | *Kernel*                                                               |   
| KERNEL END | Location of the PMM in virtual address space                           |
| ...        | (Virtual) Physical Memory Manager                                      |
| 0xC0000000 | Kernel heap start (growing up)                                         |
| 0xFFF00000 | MMIO mappings (growing down)                                           |
| 0xFFFF0000 | High location of the vector table                                      |

***Note:*** Both the MMIO mapping and kernel heap occupy the same gigabyte. 
The sum of the two can't exceed 1 gigabyte and when the kernel heap grows to touch the MMIO top, the heap is full.
