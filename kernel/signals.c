// Contributors: Andrew Stepek, Michael Brennan, and Matthew Stromberg


#include "include/signals.h"
#include "include/mem_alloc.h"

#define SIGKILL		9
#define SIGUSR		10
#define SIGPRINT	11
#define SIGTERM		15

struct sig
{
	uint32_t signal_type;
	uint32_t pid;
	struct sig *next;
} *front = '\0', *rear = '\0';

int isEmpty()
{
	// checks if queue is empty
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
	// inserts a signal into the queue
	sig struct *temp = new_sig;
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
	if(!isempty())
	{
		// removes a signal from the queue
		struct sig *temp;
		temp = front;
		front = front->next;
		temp->next = '\0';
		if(!front)
		{
			rear = '\0';
		}
		return temp;
	}
}


/*
void signal_mask (char *type, int index, uint32_t PID)
{
	pcb* pcb_t = get_PCB(PID);
	pcb_t.mask[index]->type = type;	
}
*/

void signal_create(uint32_t signal_type, uint32_t pid)
{
	// takes parameters, then makes the signal and inserts it into queue
	struct sig *new_struct = malloc(sizeof(struct sig));
	new_struct->signal_type = signal_type;
	new_struct->pid = pid;
	insert(new_struct);
}

void signal_handler(struct sig *signal)
{
	switch(signal->sig_type)
	{
		case SIGKILL:
			process_destroy(signal->pid);
			break;
		case SIGUSR:
			process_create(signal->pid);
			break;
		case SIGPRINT:
			print_process_state(signal->pid);
			break;
		case SIGTERM:
			free_PCB(get_PCB(signal->pid));
			break;
	}
}

void signal_queue_call()
{
	if(isempty()) 
	{
		break;
	}
	// simply removes a signal from the queue and handles it
	signal_handler(remove());
}
