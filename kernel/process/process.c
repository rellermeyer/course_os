#include "../include/process.h"
#include "../include/klibc.h"
#include "../include/file.h"
#include "../include/loader.h"
#include "../include/vm.h"
#include "../include/elf.h"

#include <global_defs.h>

static uint32_t GLOBAL_PID;

uint32_t sample_func(uint32_t);

int process_global_init() {
	pcb_table = kmalloc(MAX_PROCESSES * 4);
	os_memset(pcb_table, 0, sizeof(int) * MAX_PROCESSES);
	GLOBAL_PID = 0;
	return 0;
}

pcb * process_create_from_file(char * file, int argc, char ** argv) {

#define START 0x20000
#define PROC_LOCATION 0x9ff00000

	struct stats fstats;
	int fd = kopen(file, 'r');
	uint32_t start = START + PROC_LOCATION;
	uint32_t len;

	get_stats(file, &fstats);
	len = fstats.size;

	int i;
	for (i = 0; i < (len / BLOCK_SIZE) + 1; i++) {
		uint32_t *v = (uint32_t*) (start + (i * BLOCK_SIZE));
		int retval = vm_allocate_page(KERNEL_VAS, (void*) v, VM_PERM_USER_RW);
		if (retval)
			os_printf("create_from_file: %d for %X\n", retval, v);
	}

	int counter = 0;
	int* location = (int*) start;
	while (counter < len) {
		kread(fd, location, 4);
		location += 1;
		counter += 4;
	}

//	kclose(fd);

	pcb * p = process_create((uint32_t*) start, len, argc, argv);
	process_init(p);

	return p;
}

/*Spring 2015 course_os: Sathya Sankaran, Rakan Stanbouly, Jason Sim

 creates a process and initializes the PCB
 returns pcb pointer upon success
 returns 0 if there is no more room in pcb table
 file_p is a file pointer that we will create the process with */
pcb* process_create(uint32_t* file_p, uint32_t len, int argc, char ** argv) {

	uint32_t* free_space_in_pcb_table = process_next_free_slot_in_pcb_table();

	//This used to be == 0, which doesn't seem correct
	if (free_space_in_pcb_table != 0) {
		pcb* pcb_pointer = (pcb*) kmalloc(sizeof(pcb));

		pcb_pointer->len = len;
		pcb_pointer->start = (uint32_t) file_p;
		pcb_pointer->argv = argv;
		pcb_pointer->argc = argc;

		//Create the process VAS here so that we can use it when allocating process memory
		pcb_pointer->stored_vas = vm_new_vas();

		//Load the file. This function returns the parsed ELF header.
		Elf_Ehdr* success = (Elf_Ehdr*) load_file(pcb_pointer, file_p);

		if (!success) {
			return (pcb*) -1;
		}

		// fill the free space with a pcb pointer
		*free_space_in_pcb_table = (uint32_t) pcb_pointer;
		//initialize PCB		
		pcb_pointer->PID = ++GLOBAL_PID;
		//4-13-15: function pointer should point to main() of file pointer.
		//         TODO: Eventually should be able to pass parameters. Put them on the stack (argv/argc)
		pcb_pointer->R15 = success->e_entry;
		pcb_pointer->current_state = PROCESS_NEW;
		pcb_pointer->has_executed = 0;

		return pcb_pointer;

	} else {
		os_printf("Out of memory in pcb table\n");
		return 0;
	}
}

//Cycles through pcb table and returns next free space
//If there is space, returns a pointer to the space
//returns 0 if no free space is available
uint32_t* process_next_free_slot_in_pcb_table() {
	uint32_t* current_address = pcb_table;
	uint32_t i;
	for (i = 0; i < MAX_PROCESSES; ++i) {
		if ((*(int*) current_address) == 0) {
			return current_address;
		}
		current_address += 1;
	}
	return 0;
}

//destroys process with param PID by clearing the pcb struct
//returns 1 upon success, 0 with failure
uint32_t process_destroy(int PID) {
	uint32_t* addressToClear = process_get_pcb_address(PID);
	pcb* pcb_p = process_get_pcb(PID);
	uint32_t free_success = process_free_pcb(pcb_p);
	*addressToClear = 0; //clears the pointer to the PCB

	if (free_success) {
		return 1;
	} else {
		return 0;
	}

}

//prints the addresses of the pcbs stored in the table
void process_print_pcb_table() {
	os_printf("printing pcb table\n");
	uint32_t* current_address = (uint32_t*) pcb_table;
	uint32_t i;
	for (i = 0; i < MAX_PROCESSES; ++i) {
		os_printf("%x\n", current_address);
		current_address++;
	}
}

//Prints all of the PIDs in the pcb table
void process_print_pid() {

	uint32_t* current_address = pcb_table;
	uint32_t i;
	for (i = 0; i < MAX_PROCESSES; i++) {
		if ((*current_address) != 0) {
			// debug
			// os_printf("curr addr: %x\n", current_address);
			// os_printf("contents: %x\n", *current_address);

			pcb* temp_pcb = (pcb*) *current_address;
			os_printf("PID: %d\n", temp_pcb->PID);
		}
		current_address++;
	}
}

/* Returns a pointer to a pcb of process with @PID,
 or 0 if no process with PID exists.
 */
pcb* process_get_pcb(uint32_t PID) {

	//search for process in pcb table
	int i;
	uint32_t* current_address = pcb_table;

	for (i = 0; i < MAX_PROCESSES; ++i) {
		if ((*current_address) != 0) {
			pcb* pcb_p = (pcb*) *current_address;
			if (pcb_p->PID == PID) {
				return pcb_p;
			}
		}
		current_address++;
	}
	os_printf("Could not find process with PID: %d\n", PID);
	return 0;
}

/* returns a pointer to the address of a pcb in the table
 given the PID or if the PID is invalid
 */
uint32_t* process_get_pcb_address(uint32_t PID) {
	if (PID <= 0) {
		os_printf("Invalid PID of: %d, exiting.", PID);
		return 0;
	}

	int i;
	uint32_t* current_address = pcb_table;

	for (i = 0; i < MAX_PROCESSES; ++i) {
		if ((*current_address) != 0) {
			pcb* pcb_p = (pcb*) *current_address;
			if (pcb_p->PID == PID) {
				return current_address;
			}
		}
		current_address++;
	}

	os_printf("Did not find PID: %d\n", PID);
	return 0;
}

// this will 0 out everything in a PCB 
// accepts a pointer to a PCB
// returns 1 if successfully frees a pcb
uint32_t process_free_pcb(pcb* pcb_p) {
	if (pcb_p == 0) {
		os_printf("Can not free. Not a valid PCB.\n");
		return 0;
	}
	pcb_p->argv = 0;
	pcb_p->PID = 0;

	return 1;
}

/* executes a process function
 return PID upon success
 return 0 upon failure
 */
uint32_t process_execute(pcb* pcb_p) {

	if (!pcb_p) {
		os_printf("Cannot execute process. Exiting.\n");
		return 0;
	}

	//Copy the current process's program counter to the new process's return register
	//The new process will use R14 to return to the parent function
	pcb_p->R14 = pcb_p->R15;

	//assert(1==2 && "process.c - We're stopping right after loading process state.");
	//4-15-15: Since execute_process is for new processes only, stored_vas must be empty 
	// assert(!pcb_p->stored_vas && "Assert error: trying to enter execute_process with already initialized process!");
	//4-13-15: Create new virtual address space for process and switch into it
	// Let's get a simple argc/argv layout going at 0x9f000000
	// Stick the program name at stack_base
	pcb_p->has_executed = 1;

	//Set state to running, this should be modified when the process is tossed into wait queues, etc
	//Check header file for a list of states
	pcb_p->current_state = PROCESS_RUNNING;
	pcb p = *pcb_p;

	vm_enable_vas(pcb_p->stored_vas);

	//This will overwrite all our operating registers with the ones saved in the struct.
	//As soon as this is called the processor will start executing the new process.

	process_set_umode_sp(p.R13);

	process_load_state((jmp_buf*) &p.R0);

	while (1)
		;
	return pcb_p->PID;
}

//test function to see if execute process works correctly.
uint32_t sample_func(uint32_t x) {
	os_printf("Sample function!! From process with PID: %d\n", x);
	return 0;
}

void __process_init_vas(pcb* pcb_p);
void __process_init_heap(pcb* pcb_p);
void __process_init_stack(pcb* pcb_p);

void process_init(pcb * pcb_p) {
	__process_init_vas(pcb_p);
	__process_init_stack(pcb_p);
	__process_init_heap(pcb_p);
}

// Copy over from kernel to user space
void __process_init_vas(pcb* pcb_p) {
	int retval = 0;

	// --------------
	// | S   | D2   |  S: src
	// | D   |      |  D: dest
	// --------------

	// incase it was already mapped
	for (int i = 20; i < 40; i++) {
		uint32_t *v = (uint32_t*) (pcb_p->start + (i * BLOCK_SIZE));
		retval = vm_free_mapping(KERNEL_VAS, (void*) v);

		if (retval) {
			ERROR("__process_init_vas: vm_free_mapping error code: %d\n",
					retval);
			break;
		}
	}

	for (int i = 20; i < 40; i++) {
		uint32_t *v = (uint32_t *) (pcb_p->start + ((i - 20) * BLOCK_SIZE));
		retval = vm_allocate_page(pcb_p->stored_vas, (void*) v,
		VM_PERM_USER_RW);
		if (retval) {
			ERROR("__process_init_vas: vm_allocate_page error code: %d\n",
					retval);
			break;
		}

		retval = vm_map_shared_memory(KERNEL_VAS,
				(void*) pcb_p->start + (i * BLOCK_SIZE), pcb_p->stored_vas,
				(void*) v, VM_PERM_PRIVILEGED_RW);
		if (retval) {
			ERROR(
					"__process_init_vas: vm_map_shared_memory error code: %d at [%d]\n",
					retval, i);
			break;
		}

	}

	int *copyIn = (int*) pcb_p->start;
	int counter = 0;
	uint32_t * v = (uint32_t*) (pcb_p->start + 20 * BLOCK_SIZE);

	while (counter < pcb_p->len) {
		*v = *copyIn;
		copyIn += 1;
		v += 1;
		counter += 4;
	}

	for (int i = 0; i < 40; i++) {
		uint32_t *v = (uint32_t*) (pcb_p->start + (i * BLOCK_SIZE));
		retval = vm_free_mapping(KERNEL_VAS, (void*) v);
		if (retval) {
			ERROR("__process_init_vas: vm_free_mapping error code: %d\n",
					retval);
			break;
		}
	}
}

//Initial page allocation for process stack in VAS
//Allows for a variety of stack limits
void __process_init_stack(pcb * pcb_p) {
	int retval = 0;

	for (int i = 0; i < (STACK_SIZE / BLOCK_SIZE); i++) {
		retval = vm_allocate_page(pcb_p->stored_vas,
				(void*) (STACK_BASE + (i * BLOCK_SIZE)), VM_PERM_USER_RW);
		if (retval) {
			ERROR("__process_init_stack: vm_allocate_page error code: %d\n",
					retval);
			break;
		}

		retval = vm_map_shared_memory(KERNEL_VAS,
				(void*) (STACK_BASE + (i * BLOCK_SIZE)), pcb_p->stored_vas,
				(void*) (STACK_BASE + (i * BLOCK_SIZE)), VM_PERM_PRIVILEGED_RW);
		if (retval) {
			ERROR("__process_init_stack: vm_map_shared_memory error code: %d\n",
					retval);
			break;
		}
	}

	uint32_t *stack_top = (uint32_t*) STACK_TOP;
	/*stack_top[-1] = 0;
	 stack_top[-2] = 0;
	 stack_top[-3] = 0;
	 stack_top[-4] = 0;
	 stack_top[-5] = STACK_BASE;
	 stack_top[-6] = 1;*/

	// Stick a NULL at STACK_TOP-sizeof(int*)
	int argc_index = -5 - pcb_p->argc;
	int argv_index = argc_index + 1;
	uint32_t argv_p = (uint32_t) STACK_BASE;

	stack_top[argc_index] = pcb_p->argc;

	for (int i = 0; i < pcb_p->argc; i++) {
		uint32_t arg_len = os_strlen(pcb_p->argv[i]);
		os_strcpy((char*) argv_p, pcb_p->argv[i]);
		stack_top[argv_index++] = argv_p;
		argv_p += arg_len;
		*((char*) argv_p) = '\0';
		argv_p += 1;
	}

	// We need to set sp (r13) to stack_top - 12
	pcb_p->R13 = STACK_TOP + argc_index * sizeof(char*);

// #define TEST_ARGS
#ifdef TEST_ARGS
	uint32_t * args = (uint32_t*) (pcb_p->R13);
	char ** argv = (char **) (args + 1);

	for (int i = 0; i < *args; i++) {
		LOG("process_init: arg[%d][%s][0x%X]\n", i, argv[i], &argv[i]);
	}
#endif

	for (int i = 0; i < (STACK_SIZE / BLOCK_SIZE); i++) {
		retval = vm_free_mapping(KERNEL_VAS,
				(void*) (STACK_BASE + (i * BLOCK_SIZE)));
		if (retval) {
			ERROR("__process_init_stack: vm_free_mapping error code: %d\n",
					retval);
			break;
		}
	}
}

void __process_init_heap(pcb* pcb_p) {
	//Initial page allocation for a process heap in VAS
	int retval = vm_allocate_page(pcb_p->stored_vas, (void*) HEAP_BASE,
	VM_PERM_USER_RW);
	if (retval) {
		ERROR("__process_init_heap: vm_allocate_page error code: %d\n", retval);
	}
}
