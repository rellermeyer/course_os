/*
Signals
*/
/*
SIGKILL
SIGUSR
SIGTERM
SIGMASK
SIGPRINT
*/

#include "include/signal.h"



void queue_signal (String type, int PID)
{
	while ((currentSig->nextSig) != 0)
	{
		nextSig = currentSig;
	}
	struct sig newSig;
	newSig->type = type;
	newSig->signalling_processor_PID = PID;
	newSig->nextSig = 0;
}

void kill_signal (int PID)
{

}

void user_signal (int PID)
{

}

void term_signal (int PID)
{

}

void mask_signal (int PID)
{

}
