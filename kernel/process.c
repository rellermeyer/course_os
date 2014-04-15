#include "include/process.h"
#include "include/klibc.h"

int init_all_processes() {
    pcb_table = mem_alloc(MAX_PROCESSES);
	GLOBAL_PID = 0;
} 

//creates a process and initializes the PCB
//returns pcb pointer upon success
//returns 0 if there is no more room in pcb table
pcb* process_create(uint32_t starting_address, char* process_name) {

	uint32_t* free_space_in_pcb_table = next_free_slot_in_pcb_table();
	
	if(*free_space_in_pcb_table == 0) {
		pcb* pcb_pointer = (pcb*) mem_alloc(sizeof(pcb));
		*free_space_in_pcb_table = (uint32_t) pcb_pointer; //fill the free space with a pcb pointer

		//initialize PCB		
		pcb_pointer->name = process_name;
		pcb_pointer->PID = ++GLOBAL_PID;
		pcb_pointer->function = sample_func;
		
		return pcb_pointer;
	} else {
		print_uart0("Out of memory in pcb table");
		return 0;
	}
}

//Cycles through pcb table and returns next free space
//If there is space, returns a pointer to the space
//returns -1 if no free space is available
uint32_t* next_free_slot_in_pcb_table() {
	uint32_t* current_address = pcb_table;
	uint32_t i;
	for(i = 0; i < MAX_PROCESSES; ++i) {
		
		if((*(int*)current_address) == 0){
			return current_address;
		}
		current_address += 1;
	}
	return -1;
}

void save_process_state(int PID){
	uint32_t* process_to_save = get_address_of_PCB(PID);
	pcb* pcb_p = get_PCB(PID);
	uint32_t reg_0 = 0;
	os_printf("reg 0 = %x\n", reg_0);
	uint32_t reg_1;
	uint32_t reg_2;
	uint32_t reg_3;
	uint32_t reg_15;
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
}

//destroys process with param PID by clearing the pcb struct
//returns 1 upon success, 0 with failure
uint32_t process_destroy(int PID) {
	uint32_t* addressToClear = get_address_of_PCB(PID);
	pcb* pcb_p = get_PCB(PID);
	uint32_t free_success = free_PCB(pcb_p);
	*addressToClear = 0; //clears the pointer to the PCB
	
	if(free_success){
		return 1;
	} else {
		return 0;
	}

}

//prints the addresses of the pcbs stored in the table
void print_pcb_table() {
	os_printf("printing pcb table\n");
	uint32_t* current_address = (uint32_t*) pcb_table;
	uint32_t i;
	for(i = 0; i < MAX_PROCESSES; ++i) {
		os_printf("%x\n", current_address);
		current_address++;
	}
}

//Prints all of the PIDs in the pcb table
void print_PID() {
	
	uint32_t* current_address = pcb_table;
	uint32_t i;
	for(i = 0; i < MAX_PROCESSES; i++) {
		if((*current_address) != 0) {
			// debug
			// os_printf("curr addr: %x\n", current_address);
			// os_printf("contents: %x\n", *current_address);

			pcb* temp_pcb = (pcb*) *current_address;
			os_printf("PID: %d\n", temp_pcb->PID);
		}
		current_address++;
	}
}


//returns a pointer to a pcb of process with @PID
pcb* get_PCB(uint32_t PID) {
	
	//search for process in pcb table
	int i;
	uint32_t* current_address = pcb_table;

	for (i = 0; i < MAX_PROCESSES; ++i) {
		if((*current_address) != 0) {
			pcb* pcb_p = (pcb*) *current_address; 
			if(pcb_p->PID == PID) {
				return pcb_p;
			}
		}
		current_address++;
	}
	os_printf("Could not find process with PID: %d\n", PID);
	return 0;
}

//returns a pointer to the address of a pcb in the table given the PID
//returns 0 if the PID is invalid
uint32_t* get_address_of_PCB(uint32_t PID) {
	if(PID <= 0) {
		os_printf("Invalid PID of: %d, exiting.", PID);
		return 0;
	}

	int i;
	uint32_t* current_address = pcb_table;

	for (i = 0; i < MAX_PROCESSES; ++i) {
		if((*current_address) != 0) {
			pcb* pcb_p = (pcb*) *current_address; 
			if(pcb_p->PID == PID) {
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
uint32_t free_PCB(pcb* pcb_p) {
	if(pcb_p == 0) {
		os_printf("Can not free. Not a valid PCB.\n");
		return 0;
	}
	pcb_p->name = 0;
	pcb_p->PID = 0;

	return 1;
}


//executes a process function
//return 1 upon success
//return 0 upon failure
uint32_t execute_process(pcb* pcb_p) {
	if(!pcb_p) {
		os_printf("Cannot execute process. Exiting.\n");
		return 0;
	}
	pcb_p->function(pcb_p->PID);
	return 1;
}

//test function to see if execute process works correctly.
void sample_func(uint32_t x) {
	os_printf("Sample function!! From process with PID: %d\n", x);
}

