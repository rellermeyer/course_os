#include "include/process.h"
#include "include/klibc.h"

int init_all_processes() {
    pcb_table = mem_alloc(MAX_PROCESSES);
	GLOBAL_PID = 0;
} 

pcb* process_create(uint32_t starting_address, char* process_name) {

	uint32_t* location_in_pcb_table = next_free_slot_in_pcb_table();
	
	if(*location_in_pcb_table == 0) {
		pcb* pcb_pointer = (pcb*) mem_alloc(sizeof(pcb));
		*location_in_pcb_table = (uint32_t) pcb_pointer;
		// os_printf("PCBP: %x\n", pcb_pointer);
		// os_printf("locaPCB: %x\n", location_in_pcb_table);
		pcb_pointer->name = process_name;
		pcb_pointer->PID = ++GLOBAL_PID;
		//os_printf("addr PID: %x\n", &(new_pcb));
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


//destroys process with param PID by clearing the pcb struct
//returns 1 upon success, 0 with failure
uint32_t process_destroy(int PID) {
	//search for process in pcb table
	
	
	uint32_t* addressToClear = get_address_of_PCB(PID);
	pcb* pcb_p = get_PCB(PID);
	free_PCB(pcb_p);
	*addressToClear = 0;
	
	//int i
	//uint32_t* current_address = pcb_table;
	// for (i = 0; i < MAX_PROCESSES; ++i) {
		
	// 	if((*current_address) != 0) {
	// 		pcb* temp_pcb = (pcb*) *current_address; 
			
	// 		if(temp_pcb->PID == PID) {
	// 			temp_pcb->name = 0;
	// 			temp_pcb->PID = 0;
	// 			*current_address = 0;
	// 			return 1;
	// 		}	
	// 	}
	// 	current_address++;
	// }
	return 0;
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


