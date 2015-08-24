//Contributors: Andrew Stepek, Michael Brennen, and Matthew Stromberg

/*
 Signals
 */
/*
 SIGKILL
 SIGUSR
 SIGTERM
 */

#include "signals.h"
#include "process.h"

void signal_mask(char * type, int index, uint32_t PID)
{
	pcb* pcb_t = get_PCB(PID);
	pcb_t.mask[index]->type = type;
}

