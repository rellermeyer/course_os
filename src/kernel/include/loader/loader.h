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

/**
 * A union of structures used for passing information in when calling the loader
 * main function.
 *
 * For loading a static executable, it is expected to pass the information
 * as a static_exec_result structure which should contain a pointer to a
 * Process Control Block which contains the vas structure to fill with allocated
 * pages, and a structure for the created stack and heap pointers and the entry
 * address of execution of the loaded static executable.
 *
 * This union structure is meant to provide extensibility to the current loader implementation.
 * For a newly implemented type of elf file, a person can add a corresponding input/output
 * structure to the union and use it in the specific handling of his type of ELF file.
 *
 */
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
 *  The main entrypoint into the loader.
 *
 *  First, it checks if the provided file is indeed an ELF
 *  file by validating the 'ELF magic sequence'. On fail,
 *  it returns an error code.
 *
 *  Then the function determines the type of the ELF file
 *  and calls the necessary function for handling it,
 *  propagating back its result.
 *
 * @param file  :   A pointer to the ELF file to be validated
 *                   and processed.
 *
 * @param input :   This argument is meant to be used for all
 *                   types of ELF files, containing the input/
 *                   output data when handling them. It is
 *                   interpreted in a different way for every
 *                   different type of an ELF file.
 *
 * @return          0 on success, negative loader error code otherwise.
*/
int load_elf_file(void *file, union loader_result *input);

/**
 *  The entrypoint to the loading procedure when you
 * want to create a new process from a static executable
 * - an EXEC ELF file. It takes a pointer to the ELF file
 * that should already be loaded into main memory for reading,
 * a pointer to a PCB structure whose address space field
 * to update, and a pointer to a structure for the resulting
 * stack and heap pointers and the executable entry point after
 * its process image is created.
 *
 * The function first parses the ELF file header, checking
 * if it conforms to requirements related to the specific
 * architecture. After that, it uses the process_program_header_table
 * function to allocate pages for all the process segments. Finally,
 * on successful return the PCB struct should have its vas structure
 * filled and the stackAndHeapAndEntry structure should contain
 * the needed information defined by the description of the
 * structure itself.
 *
 * If an error is encountered, the function returns a negative
 * integer for the corresponding loader error code.
 *
 * @param PCB   :   A pointer to a process control block where
 *                   the address space is to be saved.
 * @param stackAndHeapAndEntry   :  A pointer to a structure where
 *                                    to pass the resulting stack and heap
 *                                    pointers, as well as the entrypoint
 *                                    of the process.
 *
 * @param file  :   A pointer to the EXEC ELF file to be read and
 *                   from which a process image is to be created.
 *
 * @return          0 on success, negative loader error code otherwise.
 *
*/
int load_static_executable(void * file, struct ProcessControlBlock * PCB, stack_and_heap_and_entry * stackAndHeapAndEntry);


/**
 *  A function to go through the segment header table,
 *  validate all headers and allocate virtual address space
 *  for the LOAD segments, the stack and the heap.
 *
 *  It goes element-wise through the program header table,
 *  validates the program header information, where on failure it
 *  frees the address space structure and exits with an error code,
 *  allocates pages for the memory size required, and copies
 *  the segment data from the file into the pages, with possible
 *  zero-padding.
 *
 *  Finally, it allocates pages for the stack and the heap,
 *  also saving their starting addresses in the passed
 *  stack_and_heap_and_entry structure.
 *
 * @param vasToFill   :     The structure used for the virtual address
 *                           space information for the new process. Should
 *                           be used when allocating pages and freeing pages
 *                           in case of failure.
 *
 * @param stackAndHeap :    The structure that should be filled with
 *                           the stack and heap pointers on successful return.
 * @param file     :        The file from which to read the program/segment data.
 * @param phtable  :        The program/segment header table - part of the file
 *                           pointed to by the 'file' parameter.
 * @param t_size   :        The size of the program/segment header table.
 * @return                  0 on successful validation and page allocation for the
 *                           segments, negative loader error code otherwise.
 */
int process_program_header_table(struct vas2 * vasToFill, stack_and_heap_and_entry * stackAndHeap, void * file, Elf32_ProgramHeader * phtable, Elf32_Word t_size);