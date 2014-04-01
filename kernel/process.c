#include "include/process.h"

//process memory resides starting at 0X20000 

int main() {
	return 0;
}

int init_all_processes() {
	pcb_table = mem_alloc(32);
} 

int process_create(uint32_t starting_address, char * process_name) {
	uint32_t* location_in_pcb_table = next_free_slot_in_pcb_table();
	if(location_in_pcb_table != 0) {
		int * pcb_pointer = mem_alloc(sizeof(pcb));
		*location_in_pcb_table = pcb_pointer;
	}
	else {
		print_uart0("Out of memory in pcb table");
		return -1;
	}


}

uint32_t * next_free_slot_in_pcb_table() {
	uint32_t* current_address = pcb_table;
	uint32_t i;
	for(i = 0; i < MAX_PROCESSES; i++) {
		current_address += i * 8;
		if((current_address) == 0)
			return current_address;
	}
	return (uint32_t *) -1;
}