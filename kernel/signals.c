//Contributors: Andrew Stepek, Michael Brennen, and Matthew Stromberg

/*
Signals
*/
/*
SIGKILL
SIGUSR
SIGTERM
*/

#include "include/signals.h"
#include "include/process.h"

void signal_mask (char * type, int index, uint32_t PID)
{
	pcb* pcb_t = process_get_pcb(PID);
	pcb_t.mask[index]->type = type;	
}

