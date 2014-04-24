#include <stdint.h>
#include "include/mmap.h"
#include "include/process.h"

typedef struct signal
{

	String type;
	uint32_t signalling_processor_PID;
	int * next_signal;
} sig;

struct sig firstSig;
firstSig->type = begin;
firstSig->signalling_processor_PID = 0;
firstSig->next_signal = 0;

struct sig currentSig = firstSig; 
