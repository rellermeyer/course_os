
# Virtual Memory

## Description

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
