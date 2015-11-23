#include "global_defs.h"
#include "process.h"
#include "klibc.h"
#include "mem_alloc.h"
#include "loader.h"
#include "vm.h"
#include "elf.h"
#include "drivers/timer.h"

static uint32_t GLOBAL_PID;

int init_all_processes()
{
	pcb_table = kmalloc(MAX_PROCESSES * 4);
	os_memset(pcb_table, 0, sizeof(int) * MAX_PROCESSES);
	GLOBAL_PID = 0;
	return 0;
}

/*Spring 2015 course_os: Sathya Sankaran, Rakan Stanbouly, Jason Sim

 creates a process and initializes the PCB
 @param file pointer to location in memory of file
 @return pcb pointer upon success
 @return 0 if there is no more room in pcb table */
pcb* process_create(uint32_t* file_p)
{

	uint32_t* free_space_in_pcb_table = next_free_slot_in_pcb_table();

	//This used to be == 0, which doesn't seem correct
	if (free_space_in_pcb_table != 0)
	{
		pcb* pcb_pointer = (pcb*) kmalloc(sizeof(pcb));

		//Create the process VAS here so that we can use it when allocating process memory
		pcb_pointer->stored_vas = vm_new_vas();

		//Load the file. This function returns the parsed ELF header.
		Elf_Ehdr* success = (Elf_Ehdr*) load_file(pcb_pointer, file_p);

		if (!success)
		{
			return (pcb*) -1;
		}

		//Debug, should be removed once scheduler works
		os_printf("THIS IS R13: %X \n", pcb_pointer->R13);

		//fill the free space with a pcb pointer
		*free_space_in_pcb_table = (uint32_t) pcb_pointer;
		//initialize PCB		
		pcb_pointer->PID = ++GLOBAL_PID;
		//4-13-15: function pointer should point to main() of file pointer.
		//         TODO: Eventually should be able to pass parameters. Put them on the stack (argv/argc)
		pcb_pointer->R15 = success->e_entry;

		os_printf("%X ENTRY: %X \n", file_p, success->e_entry);

		pcb_pointer->current_state = PROCESS_NEW;

		pcb_pointer->has_executed = 0;
		return pcb_pointer;

	}
	else
	{
		os_printf("Out of memory in pcb table\n");
		return 0;
	}
}

/*
 Goes through PCB Table and returns next free space
 @return 0 if no free space
 @return pointer to space in memory for PCB
 */
uint32_t* next_free_slot_in_pcb_table()
{
	uint32_t* current_address = pcb_table;
	uint32_t i;
	for (i = 0; i < MAX_PROCESSES; ++i)
	{
		if ((*(int*) current_address) == 0)
		{
			return current_address;
		}
		current_address += 1;
	}
	return 0;
}

/*
 Saves all of the Registers on the machine to the PCB
 @param Process ID
 @return 0 if failed
 @return 1 for success
 */
uint32_t save_process_state(uint32_t PID)
{
	uint32_t* process_to_save = get_address_of_PCB(PID);
	pcb* pcb_p = get_PCB(PID);

	if (((uint32_t) process_to_save) == -1 || pcb_p == 0)
	{
		os_printf("Invalid PID in load_process_state");
		return 0;
	}

	asm("MOV %0, r0":"=r"(pcb_p->R0)::);
	asm("MOV %0, r1":"=r"(pcb_p->R1)::);
	asm("MOV %0, r2":"=r"(pcb_p->R2)::);
	asm("MOV %0, r3":"=r"(pcb_p->R3)::);
	asm("MOV %0, r4":"=r"(pcb_p->R4)::);
	asm("MOV %0, r5":"=r"(pcb_p->R5)::);
	asm("MOV %0, r6":"=r"(pcb_p->R6)::);
	asm("MOV %0, r7":"=r"(pcb_p->R7)::);
	asm("MOV %0, r8":"=r"(pcb_p->R8)::);
	asm("MOV %0, r9":"=r"(pcb_p->R9)::);
	asm("MOV %0, r10":"=r"(pcb_p->R10)::);
	asm("MOV %0, r11":"=r"(pcb_p->R11)::);
	asm("MOV %0, r12":"=r"(pcb_p->R12)::);
	asm("MOV %0, r13":"=r"(pcb_p->R13)::);
	asm("MOV %0, r14":"=r"(pcb_p->R14)::);
	asm("MOV %0, r15":"=r"(pcb_p->R15)::);

	return 1;

}

/*
 Loads registers using values in pcb
 @param Process ID
 @param PID
 @return Returns 0 if successful

 */
uint32_t load_process_state(uint32_t PID)
{
	uint32_t* process_to_load = get_address_of_PCB(PID);
	pcb* pcb_p = get_PCB(PID);

	if (process_to_load == 0 || pcb_p == 0)
	{
		os_printf("Invalid PID in load_process_state");
		return 0;
	}
	//while(1);
	asm("MOV r0, %0"::"r"(pcb_p->R0):);
	asm("MOV r1, %0"::"r"(pcb_p->R1):);
	asm("MOV r2, %0"::"r"(pcb_p->R2):);
	asm("MOV r3, %0"::"r"(pcb_p->R3):);
	asm("MOV r4, %0"::"r"(pcb_p->R4):);
	asm("MOV r5, %0"::"r"(pcb_p->R5):);
	asm("MOV r6, %0"::"r"(pcb_p->R6):);
	asm("MOV r7, %0"::"r"(pcb_p->R7):);
	asm("MOV r8, %0"::"r"(pcb_p->R8):);
	asm("MOV r9, %0"::"r"(pcb_p->R9):);
	asm("MOV r10, %0"::"r"(pcb_p->R10):);
	//asm("MOV r11, %0"::"r"(11):);
	asm("MOV r12, %0"::"r"(pcb_p->R12):);

	asm("MOV r13, %0"::"r"(pcb_p->R13):);

	asm("MOV r14, %0"::"r"(pcb_p->R14):);
//assert(1==11);

	asm("MOV r15, %0"::"r"(pcb_p->R15):);

	return 1;
}
/*
 Prints the register variables that a PCB contains
 @param Process ID
 @return 0 if PCB doesn't exist
 @return 1 if PCB exists
 */

uint32_t print_process_state(uint32_t PID)
{
	pcb* pcb_p = get_PCB(PID);

	if (pcb_p == 0)
	{
		return 0;
	}

	os_printf("Process State of PID: %d\n", PID);
	os_printf("reg 0 = %x\n", pcb_p->R0);
	os_printf("reg 1 = %x\n", pcb_p->R1);
	os_printf("reg 2 = %x\n", pcb_p->R2);
	os_printf("reg 3 = %x\n", pcb_p->R3);
	os_printf("reg 4 = %x\n", pcb_p->R4);
	os_printf("reg 5 = %x\n", pcb_p->R5);
	os_printf("reg 6 = %x\n", pcb_p->R6);
	os_printf("reg 7 = %x\n", pcb_p->R7);
	os_printf("reg 8 = %x\n", pcb_p->R8);
	os_printf("reg 9 = %x\n", pcb_p->R9);
	os_printf("reg 10 = %x\n", pcb_p->R10);
	os_printf("reg 11 = %x\n", pcb_p->R11);
	os_printf("reg 12 = %x\n", pcb_p->R12);
	os_printf("reg 13 = %x\n", pcb_p->R13);
	os_printf("reg 14 = %x\n", pcb_p->R14);
	os_printf("reg 15 = %x\n", pcb_p->R15);

	return 1;
}
/*
 Destroys Process with Process ID by clearing the struct
 Calls free_PCB to do this
 @return 1 if freed properly
 @return 0 if failed
 */
uint32_t process_destroy(int PID)
{
	uint32_t* addressToClear = get_address_of_PCB(PID);
	pcb* pcb_p = get_PCB(PID);
	uint32_t free_success = free_PCB(pcb_p);
	*addressToClear = 0; //clears the pointer to the PCB

	if (free_success)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}
/*
 Prints all the PCBs in the table
 */
void print_pcb_table()
{
	os_printf("printing pcb table\n");
	uint32_t* current_address = (uint32_t*) pcb_table;
	uint32_t i;
	for (i = 0; i < MAX_PROCESSES; ++i)
	{
		os_printf("%x\n", current_address);
		current_address++;
	}
}

/*
 Prints all the Process IDs
 */
void print_PID()
{

	uint32_t* current_address = pcb_table;
	uint32_t i;
	for (i = 0; i < MAX_PROCESSES; i++)
	{
		if ((*current_address) != 0)
		{
			// debug
			// os_printf("curr addr: %x\n", current_address);
			// os_printf("contents: %x\n", *current_address);

			pcb* temp_pcb = (pcb*) *current_address;
			os_printf("PID: %d\n", temp_pcb->PID);
		}
		current_address++;
	}
}

/*
 Finds the PCB that corresponds the Process ID
 @param Process ID
 @return Process Control Block on success
 @return 0 if failed
 */
pcb* get_PCB(uint32_t PID)
{

	//search for process in pcb table
	int i;
	uint32_t* current_address = pcb_table;

	for (i = 0; i < MAX_PROCESSES; ++i)
	{
		if ((*current_address) != 0)
		{
			pcb* pcb_p = (pcb*) *current_address;
			if (pcb_p->PID == PID)
			{
				return pcb_p;
			}
		}
		current_address++;
	}
	os_printf("Could not find process with PID: %d\n", PID);
	return 0;
}

/* 
 Returns pointer to PCB for corresponding Process ID
 @param Process ID
 @return Address of PCB
 @return 0 if PID isn't valid


 */
uint32_t* get_address_of_PCB(uint32_t PID)
{
	if (PID <= 0)
	{
		os_printf("Invalid PID of: %d, exiting.", PID);
		return 0;
	}

	int i;
	uint32_t* current_address = pcb_table;

	for (i = 0; i < MAX_PROCESSES; ++i)
	{
		if ((*current_address) != 0)
		{
			pcb* pcb_p = (pcb*) *current_address;
			if (pcb_p->PID == PID)
			{
				return current_address;
			}
		}
		current_address++;
	}

	os_printf("Did not find PID: %d\n", PID);
	return 0;
}

/*
 Zeroes out Name and Process ID of corresponding PCB
 @param Process Control Block
 @return 0 if PCB isn't valid
 @return 1 if successfully wiped

 */
uint32_t free_PCB(pcb* pcb_p)
{
	if (pcb_p == 0)
	{
		os_printf("Can not free. Not a valid PCB.\n");
		return 0;
	}
	pcb_p->name = 0;
	pcb_p->PID = 0;

	return 1;
}

/* 
 Moves the return location of current process into R14
 Sets the current state then loads the process registers

 @param Pointer to process control block
 @param pcb* pcb_p
 @return Process ID


 */
uint32_t execute_process(pcb* pcb_p)
{
	if (!pcb_p)
	{
		os_printf("Cannot execute process. Exiting.\n");
		return 0;
	}

	//Copy the current process's program counter to the new process's return register
	//The new process will use R14 to return to the parent function
	asm("MOV %0, r15":"=r"(pcb_p->R14)::);

	//Switch to user virtual address space, this is self explanatory

	//Should be disabled once scheduler is working to prevent spam
	os_printf("Should be VAS: %x\n", vm_get_current_vas());

	//5-1-15: The following commented stuff is obsolete and only included for work reference
	//assert(1==2 && "process.c - We're stopping right after loading process state.");
	//4-15-15: Since execute_process is for new processes only, stored_vas must be empty 
	// assert(!pcb_p->stored_vas && "Assert error: trying to enter execute_process with already initialized process!");
	//4-13-15: Create new virtual address space for process and switch into it
	// Let's get a simple argc/argv layout going at 0x9f000000
	// Stick the program name at stack_base
	vm_enable_vas(pcb_p->stored_vas);

	print_process_state(pcb_p->PID);

	pcb_p->has_executed = 1;

	//Set state to running, this should be modified when the process is tossed into wait queues, etc
	//Check header file for a list of states
	pcb_p->current_state = PROCESS_RUNNING;

	//This will overwrite all our operating registers with the ones saved in the struct.
	//As soon as this is called the processor will start executing the new process.
	load_process_state(pcb_p->PID);
	while (1)
		;
	return pcb_p->PID;
}

/*
 Allocates memory in the process VAS, and
 copies over the process to that location in memory

 @param pointer to process control block
 @param pcb* pcb_p

 */

void setup_process_vas(pcb* pcb_p)
{

	//		assert(1==15);
	for (int i = 0; i < 20; i++)
	{
		uint32_t *v = (uint32_t*) (pcb_p->start + (i * BLOCK_SIZE));
		int x = vm_allocate_page(pcb_p->stored_vas, (void*) v, VM_PERM_USER_RW);
		assert(x == 0);
		vm_map_shared_memory(KERNEL_VAS, (void*) v, pcb_p->stored_vas,
				(void*) v, VM_PERM_USER_RW);
	}

	uint32_t *copyIn = (uint32_t *) pcb_p->start;
	int counter = 0;
	uint32_t * v = (uint32_t*) pcb_p->start;
	//*v = *copyIn;
	while (counter < pcb_p->len)
	{
		*v = *copyIn;
		copyIn += 1;
		v += 1;
		counter += 4;
	}

	for (int i = 0; i < 20; i++)
	{
		uint32_t *v = (uint32_t *) (pcb_p->start + (i * BLOCK_SIZE));
		vm_free_mapping(KERNEL_VAS, (void*) v);

	}

}
/*
 Allocated memory for the process stack
 Moves arguments for argc, argv, envp, and auxp
 into stack_top

 Points stack pointer to location where stack_top would begin
 @param pointer to process control block
 @param pcb* pcb_p

 */
void init_proc_stack(pcb * pcb_p)
{
	int retval = 0;
	for (int i = 0; i < (STACK_SIZE / BLOCK_SIZE); i++)
	{
		retval = vm_allocate_page(pcb_p->stored_vas,
				(void*) (STACK_BASE + (i * BLOCK_SIZE)), VM_PERM_USER_RW);
		if (retval)
		{
			os_printf("vm_allocate_page error code: %d\n", retval);
			break;
		}
		else
		{
			os_printf(
					"A page have been allocated for process stack at vptr: 0x%x\n",
					(STACK_BASE + (i * BLOCK_SIZE)));
		}
		vm_map_shared_memory(KERNEL_VAS,
				(void*) (STACK_BASE + (i * BLOCK_SIZE)), pcb_p->stored_vas,
				(void*) (STACK_BASE + (i * BLOCK_SIZE)), VM_PERM_USER_RW);

	}

	// Stick a NULL at STACK_TOP-sizeof(int*)
	uint32_t *stack_top = (uint32_t*) STACK_TOP;
	stack_top[-1] = 0;
	stack_top[-2] = 0;
	stack_top[-3] = 0;
	stack_top[-4] = 0;
	stack_top[-5] = STACK_BASE;
	stack_top[-6] = 1;

	os_strcpy((char*) STACK_BASE, pcb_p->name);

	// We need to set sp (r13) to stack_top - 12
	pcb_p->R13 = STACK_TOP - 4 * 6;
	print_process_state(pcb_p->PID);

	for (int i = 0; i < (STACK_SIZE / BLOCK_SIZE); i++)
	{
		vm_free_mapping(KERNEL_VAS, (void*) (STACK_BASE + (i * BLOCK_SIZE)));

	}
}
void init_proc_heap(pcb* pcb_p)
{
	//from mem_alloc.c
	init_process_heap(pcb_p->stored_vas);
	os_printf("User Level Heap for Process PID %d initialized\n", pcb_p->PID);
}
