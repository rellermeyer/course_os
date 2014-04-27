//Contributors: Andrew Stepek, Michael Brennen, and Matthew Stromberg

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
};

struct sig *front = '\0';
struct sig *rear = '\0';

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

void signal_mask (char * type, int index, uint32_t PID)
{
	pcb* pcb_t = get_PCB(PID);
	pcb_t.mask[index]->type = type;	
}

int raise(int sig)
{
	// raises the signal
}

void signal_handler(int sig_no)
{
/*
	switch(sig_no)
	{
	}
*/
}
