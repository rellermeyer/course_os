#include <elf.h>

// Function declarations

/* The main entrypoint into the loader, used when you
	want to create a new process from an ELF file. It
	takes a pointer to the ELF file that should already
	be loaded into main memory for reading.
	@return A pointer to a newly created PCB for the new process
*/
void* loadProcessFromElfFile(void * file);


