# Process Creation - Overview of Steps


    **Attention!!!** Currently this document will be more focused on the procedure for loading normal independent executable files. The document will/should be extended in the future for other types of ELF files as well.

## Target ELF files common characteristics

The ELF files that the loader expects have to have the following common fixed values for their fields:

- **ELF Type**      - 32-bit                - 0x01
- **OS ABI**        - To be determined ( Possibly System V as it is common to be used regardless the target platform)
- **Machine ISA**   - ARM ( not AArch64 )   - 0x28
- **Endianess**     - little endian         - 0x01
- **Version**       - always 1              - 0x01
- **Header Size**   - 54 bytes - 32-bit ELF - 0x36


## Loading Steps

1. Receive the full path to an ELF file to load into memory and possibly create a process from.
       
2. Use the File System Kernel Service to load this file in memory for reading. Get a file descriptor to read from.
       
3. Read and validate the ELF header

    1. Validate the MAGIC NUMBER + “ELF” -> STOP if fail
    2. Validate the ELF 32 type, OS ABI, ISA, Endianess, Version, Header Size
    3. Extract all the needed information from the ELF Header:
	- start address offset
	- offset of program header table
	- length of program header table
	- offset of section header table
	- length of section header table
	- index into the section header table for the section names 
    4. Create a new page table context for the new process
    5. Go through the program header table (if exists)
        - Check the type of segment
          - Is it LOAD
          - (OPTIONAL) Is it valid/reserved/extra information 
        - Check the file size and memory size
        - Check the virtual start address
        - From the memory size determine how many pages have to be allocated by the VMM
        - Allocate the pages starting from the header-specified virtual address
            - For each page, use the flags in the header to specify permissions for the page - r/w/x
        **- Copy the segment contents to the allocated page memory**
          - For this part to work, we have to actually use the page tables we created
    6.  Create a heap segment
        1.  Allocate more pages 
        2.  Set some kind of heap pointer in the PCB
    7.  Create a stack segment 
        1.  Allocate more pages
        2.  Set an initial value for the stack pointer
    8.  Set the starting address of the process - either in the PCB or put it in the right place on the new user process stack 