#include <loader.h>
#include <string.h>
#include <vas2.h>
#include <vm2.h>


int load_elf_file(void *file, union loader_result *input) {

    if (file == NULL || input == NULL) return NULL_POINTER;

    // Make sure this is an Elf file
    Elf32_Header * header = (Elf32_Header *)file;
    int result = elf_validate_magic_sequence(header);
    if (result < 0) return result;

    INFO("Loader: Input file is indeed an ELF file.");

    // Determine what to do based on the type of the ELF file
    enum Elf_Type e_type = header->e_type;
    switch(e_type) {
        case ET_EXEC:
            INFO("Loader: ELF file type is EXEC: Supported!\n");
            return load_static_executable(file, input->static_exec_result.PCB, input->static_exec_result.stackAndHeapAndEntry);
        case ET_NONE:
            FATAL("ELF file type is NONE!");
        case ET_REL:
            FATAL("ELF file type is REL: Not allowed in the loader!");
        case ET_DYN:
            FATAL("ELF file type is DYN: Dynamic executables and shared object files are not supported!");
        default:
            FATAL("ELF file type not recognised");
    }
}

int load_static_executable(void * file, struct ProcessControlBlock * PCB, stack_and_heap_and_entry * stackAndHeapAndEntry) {

    // validate the input
    if(PCB == NULL || stackAndHeapAndEntry == NULL) return NULL_POINTER;

    Elf32_Header * header = (Elf32_Header *)file;

    Elf elf_info;

    INFO("Parsing elf header ...");

    int result = elf_parse_header(&elf_info, header);
    if (result < 0) return INVALID_HEADER;

    // Here we check for the presence of a table that in the case
    // of the specific ELF file type found is mandatory
    // TODO: Make the check depend on the e_type field
    // Currently it just checks if the program header table
    // is present.
    if (header->e_program_header_table_offset == 0) {
        INFO("This ELF file does not have a program header table!");
        return PH_TABLE_MISSING;
    }

    INFO("Creating a new vas ...");

    // Initialize a new address space for the new process to create
    struct vas2 * new_vas = PCB->vas;

    INFO("Processing program header table ...\n");

    // Process the program(segment) header table - allocate pages and copy data for the loadable segments
    int processResult = process_program_header_table(new_vas, stackAndHeapAndEntry, file,
                                                     elf_info.programHeaderTableAddress,
                                                     elf_info.programHeaderTableLength);

    stackAndHeapAndEntry->entry = elf_info.entry;

    // If the operation was not successful, free up any data in the vas structure
    // and signal a FATAL error.
    if (processResult < 0) {
        FATAL("Creating a process image failed: Loader error code %d! \n", processResult);
    }

    // If the operation was successful, continue ...
    INFO("Creating the process image was successful!\n");
    return 0;
}

int process_program_header_table(struct vas2 * vasToFill, stack_and_heap_and_entry *stackAndHeap, void * file, Elf32_ProgramHeader * phtable, Elf32_Word t_size) {

    // Switch to the new process address space
    switch_to_vas(vasToFill);

    INFO("Switched to new vas!");
    INFO("Program header table size: %d\n", t_size);

    // Use this variable to keep track of the first free virtual address after the
    // already populated with segment data -> Then use it for the heap and the stack
    Elf32_Addr currentVirtualAddress = 0;

    // Iterate over the program headers
    for (Elf32_Word ph_index = 0; ph_index < t_size; ph_index++) {

        // Get the current program header
        Elf32_ProgramHeader * currentProgramHeader = phtable + ph_index;

        // Check if the header has valid data and whether it is supported
        int result;
        if ((result = elf_validate_program_header(currentProgramHeader)) < 0) {
            // On error clean the vas structure
            free_vas(vasToFill);
            return PH_TYPE_INVALID_OR_UNSUPPORTED;
        }

        INFO("Header has valid data.");

        // If the segment is a LOAD type, move on to allocate space for it and copy its data
        if (currentProgramHeader->program_type == EPT_LOAD) {

            /*
              Segment
              |=============                                                 |------
              |=============  <-  header->virtual_address                    | Page
              |                                                              |
              |                   \                                          |------
              |  segment           \                                         | Page
              |   data              |- header->file_size                     |
              |                    /                                         |------
              |                   /                                          | Page
              |-------------                                                 |
              |                                                              |------
              |   possible                                                   | Page
              |   padding                                                    |
              |  of zeroes                                                   |------
              |                                                              | Page
              |=============  <- header->virtual_address + memsize           |
              |=============                                                 |------
            */

            // Get the virtual address
            Elf32_Addr virtualAddress = currentProgramHeader->program_virtual_addr;

            // Use this in case padding with zeroes is needed
            Elf32_Addr segmentStartAddress = virtualAddress;

            // Find the data of the segment in the input file
            Elf32_Addr programToRead = (Elf32_Addr)((char *)file + currentProgramHeader->program_offset);

            // Get the size of the segment in the input file
            Elf32_Word fileSize = currentProgramHeader->program_filesz;

            // Get the required size of the segment in memory
            Elf32_Word memorySize = currentProgramHeader->program_memsz;
            int32_t sizeLeftToAllocate = memorySize;

            INFO("Header data: ");
            kprintf("VIRTUAL_ADDRESS: %x  -  FILE_ADDRESS: %x  -  FILE_SIZE: %d  -  MEM_SIZE: %d\n", virtualAddress, programToRead, fileSize, memorySize);

            // While there is more memory space that needs to be allocated for the segment
            // allocate a page for it and adjust the starting virtual address for the
            // next page.
            INFO("Allocating %d pages for segment ...", sizeLeftToAllocate / PAGE_SIZE + ((sizeLeftToAllocate % PAGE_SIZE) ? 0 : 1));
            while (sizeLeftToAllocate > 0) {
                allocate_page(vasToFill,virtualAddress, currentProgramHeader->flags & EPF_EXEC);
                sizeLeftToAllocate -= PAGE_SIZE;
                virtualAddress += PAGE_SIZE;
            }


            // copy the segment data into the newly allocated pages
            memcpy((void *)segmentStartAddress, (void *)programToRead, fileSize);

            // In case the memory size is bigger than the file size, pad with zeroes.
            if (fileSize < memorySize) {
                Elf32_Addr padding_address = segmentStartAddress + fileSize;
                INFO("Padding is needed of size: %d", memorySize - fileSize);
                while (padding_address < segmentStartAddress + memorySize) {
                    *(char *)padding_address = '\0';
                    padding_address++;
                }
                INFO("Padding completed!");

            }

            // Keep track of the first free virtual address after the last segment.
            currentVirtualAddress = virtualAddress;

            INFO("Pages allocated for segment and data is set up!\n");

        }
    }

    INFO("Program headers processed successfully!");
    INFO("Allocating stack and heap memory ...");

    /* Allocate pages for the stack and the heap.
       |  STACK
       |   ||  - // - 16KB currently
       |   \/
       |  -----
       |   /\
       |   ||  - // - 16KB currently
       |  HEAP
    */

    // Get the heap pointer for the new process
    Elf32_Addr heap_address = currentVirtualAddress;

    // Allocate pages for the heap - 16MB = 4KB * 2^12
    kprintf("Heap starts at address: %x\n", currentVirtualAddress);
    size_t counter = 0;
    while (counter < PROCESS_HEAP_SIZE_IN_PAGES) {
        allocate_page(vasToFill, currentVirtualAddress, false);
        currentVirtualAddress += PAGE_SIZE;
        counter++;
    }
    INFO("Pages for heap allocated successfully!");


    // Allocate pages for the stack - 16MB = 4KB * 2^12
    counter = 0;
    while (counter < PROCESS_STACK_SIZE_IN_PAGES) {
        allocate_page(vasToFill, currentVirtualAddress, false);
        currentVirtualAddress += PAGE_SIZE;
        counter++;
    }
    kprintf("Stack starts at address: %x\n", currentVirtualAddress - 4);
    INFO("Pages for stack allocated successfully!");


    INFO("Returning from the segment processing routine ...");

    // Get the stack pointer for the new process
    Elf32_Addr stack_address = currentVirtualAddress - 4;

    //Set the stack and heap pointers in the passed structure.
    stackAndHeap->stack_pointer = stack_address;
    stackAndHeap->heap_pointer = heap_address;

    return 0;
}
