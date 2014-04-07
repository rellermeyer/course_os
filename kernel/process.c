#include "include/process.h"
#include "include/klibc.h"

//process memory resides starting at 0X20000 


int init_all_processes() {
    pcb_table = mem_alloc(MAX_PROCESSES);

	//DEBUG
	//os_printf("pcb table: %x\n", pcb_table);
	GLOBAL_PID = 0;
} 

int process_create(uint32_t starting_address, char* process_name) {

	uint32_t* location_in_pcb_table = next_free_slot_in_pcb_table();
	
	if(location_in_pcb_table != 0) {
		uint32_t* pcb_pointer = (uint32_t*) mem_alloc(sizeof(pcb));
		*location_in_pcb_table = (uint32_t) pcb_pointer;

		pcb* new_pcb;
		new_pcb->name = process_name;
		new_pcb->PID = ++GLOBAL_PID;

		return (int) pcb_pointer;
	} else {
		print_uart0("Out of memory in pcb table");
		return -1;
	}
}

uint32_t* next_free_slot_in_pcb_table() {
	uint32_t* current_address = pcb_table;
	uint32_t i;
	for(i = 0; i < MAX_PROCESSES; ++i) {
		current_address += i * 8;
		if((*current_address) == 0){
			return current_address;
		}
	}
	return (uint32_t *) -1;
}

int process_destroy(int PID) {
	//search for process in pcb table
	int i;
	pcb* temp_pcb;
	for (i = 0; i < MAX_PROCESSES; ++i) {
		
	}

	return 1;
}

//prints the addresses of the pcbs stored in the table
void print_pcb_table() {
	uint32_t* current_address = pcb_table;
	uint32_t i;
	for(i = 0; i < MAX_PROCESSES; ++i) {
		current_address += i * 8;
		os_printf("%x\n", *current_address);
	}
}