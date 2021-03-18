#include <elf_new.h>
#include <vas2.h>
#include <constants.h>
#include <processControlBlock.h>


#define PROCESS_STACK_SIZE_IN_PAGES 4096
#define PROCESS_HEAP_SIZE_IN_PAGES 4096

// Function declarations

/* The main entrypoint into the loader, used when you
	want to create a new process from an ELF file. It
	takes a pointer to the ELF file that should already
	be loaded into main memory for reading.
	@return A pointer to a newly created PCB for the new process
*/
int loadProcessFromElfFile(ProcessControlBlock * PCB, void * file);

int processProgramHeaderTable(struct vas2 * vasToFill, void * file, Elf32_ProgramHeader * phtable, Elf32_Word t_size);


int printSectionNames(void * file, Elf32_SectionHeader * shtable, Elf32_Word t_size, Elf32_Half names_index); 
