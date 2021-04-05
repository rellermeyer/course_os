#include <elf.h>
#include <vas2.h>
#include <constants.h>
#include <processControlBlock.h>


#define PROCESS_STACK_SIZE_IN_PAGES 4
#define PROCESS_HEAP_SIZE_IN_PAGES 4

/**
 * A structure holding the addresses
 * of the stack and the heap after the
 * process image is created. It also
 * holds the entry point for the process
 * execution.
 */
typedef struct stack_and_heap_and_entry {
    Elf32_Addr stack_pointer;
    Elf32_Addr heap_pointer;
    Elf32_Addr entry;
} stack_and_heap_and_entry;

/**
 * Loader error codes
 */
enum LOADER_ERROR_CODE {
    NULL_POINTER=-1,
    INVALID_HEADER=-2,
    PH_TABLE_MISSING=-3,
    SH_TABLE_MISSING=-4,
    PH_TYPE_INVALID_OR_UNSUPPORTED=-5,
    SH_TYPE_INVALID_OR_UNSUPPORTED=-6
};

union loader_result {
    struct {
        ProcessControlBlock * PCB;
        stack_and_heap_and_entry * stackAndHeapAndEntry;
    } static_exec_result;

    struct {
        // TODO
    } dynamic_exec_result;

    struct {
        // TODO
    } shared_object_result;
};


// Function declarations

/**
 *  The main entrypoint into the loader, used when you
 * want to create a new process from an ELF file. It
 * takes a pointer to the ELF file that should already
 * be loaded into main memory for reading and a pointer
 * to a PCB structure whose address space field to update.
 * The function first parses the ELF file header, checking
 * if it conforms to requirements related to the specific
 * architecture. After that, it creates a new address space
 * structure and goes through the header tables, allocating pages
 * for all the segments/sections using the created address space
 * structure. If the processing of the header tables is successful,
 * the function finishes by using an external routine for creating
 * the initial saved execution state for the new process.
 *
 * @param PCB   :   A pointer to a process control block where
 *                   the address space is to be saved.
 * @param file  :   A pointer to the ELF file to be read and
 *                   from which a process image is to be created.
 *
 * @return          A pointer to a newly created PCB for the new
 *                   process.
*/
int load_elf_file(void *file, union loader_result *input);

int load_static_executable(void * file, struct ProcessControlBlock * PCB, stack_and_heap_and_entry * stackAndHeapAndEntry);


/**
 *  A function to go through the segment header table,
 *  validate all headers and allocate virtual address space
 *  for the LOAD segments, the stack and the heap.
 *
 * @param vasToFill   :     The structure used for the virtual address
 *                           space information for the new process. Should
 *                           be used when allocating pages and when copying
 *                           data into the new address space.
 * @param stackAndHeap :    The structure that should be filled with
 *                           the stack and heap pointers on successful return.
 * @param file     :        The file from which to read the program/segment data.
 * @param phtable  :        The program/segment header table - part of the file
 *                           pointed to by the 'file' parameter.
 * @param t_size   :        The size of the program/segment header table.
 * @return 0 on successful validation and page allocation for the segments, -1 otherwise.
 */
int processProgramHeaderTable(struct vas2 * vasToFill, stack_and_heap_and_entry * stackAndHeap, void * file, Elf32_ProgramHeader * phtable, Elf32_Word t_size);