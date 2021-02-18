#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <chipset.h>
#include <processControlBlock.h>
#include <interrupt.h>

void init_scheduler();

void remove(int id);
void add(ProcessControlBlock*);

void* callback();  // should pick the next process to run and set the callback again

#endif  // ! SCHEDULER_H
