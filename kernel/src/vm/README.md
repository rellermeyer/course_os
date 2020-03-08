
# Virtual Memory

## Description

## Memory map

### Physical

| Address    | Description                                                            |
| ---------- | ---------------------------------------------------------------------- |
| 0x0000000  | Interrupt Vector Table                                                 |
| 0x0004000  | Kernel L1 page table                                                   |
| 0x0008000  | Kernel start                                                           |
| ...        | *Kernel*                                                               |   
| ...        | *Kernel*                                                               |
| KERNEL END | Physical Memory Manager starting point                                 |
| ...        | Physical Memory Manager                                                |
| 0xXXXXXXXX | End of Physical memory (Device dependent, max 1GB)                     |

### Virtual 

| Address    | Description                                                            |
| ---------- | ---------------------------------------------------------------------- |
| 0x0000000  | Virtual Process Address Space                                          |
| 0x8000000  | Interrupt Vector Table (remap of the first gigabyte of phys --> virt)  |
| 0x8004000  | Kernel page table                                                      |
| 0x8008000  | Kernel start                                                           |
| ...        | *Kernel*                                                               |   
| ...        | *Kernel*                                                               |   
| KERNEL END | Location of the PMM in virtual address space                           |
| ...        | (Virtual) Physical Memory Manager                                      |
| 0xC000000  | Kernel heap start (growing up)                                         |
| 0xFFFFFFF  | MMIO mappings (growing down)                                           |

***Note:*** Both the MMIO mapping and kernel heap occupy the same gigabyte. 
The sum of the two can't exceed 1 gigabyte and when the kernel heap grows to touch the MMIO top, the heap is full.
