```
    Attention!!!    Currently this document will be more focused on the 
                    procedure for loading simple static executable files. 
                    The document will/should be extended in the future for 
                    other types of ELF files as well.
```

## Description

This module provides an implementation of an OS loader.
It is meant to work with files conforming to the Executable and Linkable Format (ELF). 

The data structures and functions defined in this module are found in the following files:
- elf.h : Structures and functions for working with elf files
- elf_common_features : Features every ELF file has to have for this OS.
- loader.h  : Structures and functions for loading files
- loaderutils.h : Helper debugging functions for ELF files.

In order to use the loader, one has to invoke the function:

`  int load_elf_file(void *file, union loader_result *input)  `
- file  - the pointer to the ELF file in memory to process
- input - depending on the type of the elf file you will process it can be interpreted as a different structure

For more information on the function, visit the loader.h file.


## Sources of Information

Elf Overview: 
 - https://wiki.osdev.org/ELF
 - https://www.youtube.com/watch?v=nC1U1LJQL8o&feature=youtu.be
 - https://man7.org/linux/man-pages/man5/elf.5.html

Object Files, Loaders and Linkers:
 - https://docs.oracle.com/cd/E23824_01/pdf/819-0690.pdf
    - ELF Application Binary Interface

## ELF

### Target ELF files common characteristics

The ELF files that the loader expects have to have the following common fixed values for their fields:

|       |           |               |
| ---   |   ---     |      ---      |
|**ELF Type**   |   32-bit          |      0x01 |
| **OS ABI**    |    To be determined ( Possibly System V as it is common to be used regardless the target platform)
| **Machine ISA**    |  ARM ( not AArch64 ) | 0x28
| **Endianess**      | little endian        | 0x01
| **Version**        |always 1              | 0x01
| **Header Size**    |54 bytes - 32-bit ELF | 0x36

## Loader

### Loader Error Codes

| Number   |   Name    |   Meaning     |
|   ---    |    ---    |     ---       |
|    -1     |   NULL_POINTER        |  Loader received NULL argument|
|    -2     |   INVALID_HEADER      |  The ELF header has invalid data      |
|    -3     |   PH_TABLE_MISSING    |  There is no program/segment header table in the ELF file |
|    -4     |   SH_TABLE_MISSING    |  There is no section header table in the ELF file |

### Loading Steps for a Static Executable ELF file

1. Read and validate the ELF header

    1. Validate the MAGIC NUMBER + “ELF” -> STOP if fail
    2. Validate the ELF 32 type, OS ABI, ISA, Endianess, Version, Header Size
    3. Extract all the needed information from the ELF Header:
    - start address offset
    - address of program header table
    - length of program header table
    - address of section header table
    - length of section header table
    - address of the section header table for the section names
    
2. Get the page table context for the new process
   
3. Go through the program header table
        - Check the type of segment
            - If it is LOAD:
                - Check the file size and memory size
                - Check the virtual start address
                - From the memory size determine how many pages have to be allocated by the Virtual Memory Manager
                - Allocate the pages starting from the header-specified virtual address
                    - For each page, use the flags in the header to specify permissions for the page - r/w/x
                    - Copy the segment contents to the allocated page memory
                    - Pad with zeroes the segment if memsize > filesize
            
4.  Create a heap segment
        1.  Allocate more pages
        2.  Determine the heap pointer as the starting address of the first allocated page
    
5.  Create a stack segment
        1.  Allocate more pages
        2.  Determine the initial stack pointer as the last address in the last allocated page
    
6.  Save the starting address of the process, the stack and the heap pointers
