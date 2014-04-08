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

uint32_t* next_free_slot_in_pcb_table() {
	uint32_t* current_address = pcb_table;
	uint32_t i;
	for(i = 0; i < MAX_PROCESSES; ++i) {
		
		if((*(int*)current_address) == 0){
			return current_address;
		}
		current_address += 1;
	}
	return (uint32_t *) -1;
}


//destroys process with param PID by clearing the pcb struct
//returns 1 upon success, 0 with failure
uint32_t process_destroy(int PID) {
	//search for process in pcb table
	int i;
	uint32_t* current_address = pcb_table;

	for (i = 0; i < MAX_PROCESSES; ++i) {
		
		if((*current_address) != 0) {
			pcb* temp_pcb = (pcb*) *current_address; 
			
			if(temp_pcb->PID == PID) {
				temp_pcb->name = 0;
				temp_pcb->PID = 0;
				*current_address = 0;
				return 1;
			}	
		}
		current_address++;
	}
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

