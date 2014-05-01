// Contributors: Andrew Stepek, Michael Brennan, and Matthew Stromberg

/*
Signals: SIGKILL, SIGUSR, SIGTERM, SIGPRINT
*/

#include "include/signals.h"
#include "include/mem_alloc.h"

struct sig
{
	uint32_t signal_type;
	uint32_t pid;
	struct sig *next;
} *front = '\0', *rear = '\0';

int isEmpty()
{
	if (front == '\0')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void insert(struct sig *new_sig)
{
	sig struct *temp = malloc(sizeof(struct sig));
	temp = new_sig;
	if (isEmpty())
	{
		temp->next = '\0';
		front = rear = temp;
	} 
	else
	{
		temp->next = '\0';
		rear->next = temp;
		rear = temp;
	}
}

struct sig *remove()
{
	struct sig *temp;
	temp = front;
	front = front->next;
	temp->next = '\0';
	free(temp);
	if(!front)
	{
		rear = '\0';
	}
}



void signal_mask (char *type, int index, uint32_t PID)
{
	pcb* pcb_t = get_PCB(PID);
	pcb_t.mask[index]->type = type;	
}

void signal_handler(struct sig *signal)
{
	switch(signal->sig_type)
	{
		case 9:
			// SIGKILL
			process_destroy(signal->pid);
			break;
		case 10:
			// SIGUSR
			process_create(signal->pid);
			break;
		case 11:
			// SIGPRINT
			print_process_state(signal->pid);
			break;
		case 15:
			// SIGTERM
			free_PCB(get_PCB(signal->pid));
			break;
	}
}
