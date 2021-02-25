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
    3.   
    4. ... To be continued. Possible TODOS
       1. Determine the locations of the program/section header table
       2. Read through the table and determine how much memory you need to allocate in pages
       3. Reserve memoru pages for the new process using the Virtual Memory Manager