#ifndef SIGNAL_H
#define SIGNAL_H

//Contributors: Michael Brennan, Andrew Stepek, and Matthew Stromberg

#include <stdint.h>
#include "mmap.h"
#include "process.h"

void signal_create(uint32_t signal_type, uint32_t pid);		// creates signal given type and pid and inserts it into queue
void signal_queue_call();					// removes signal from queue and handles it

#endif
