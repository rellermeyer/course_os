#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <chipset.h>
#include <processControlBlock.h>
#include <interrupt.h>

void init_scheduler();

void sleep(int id, int sleepTime);
void remove(int id);

void* getNext();  // should pick the next process to run and set the callback again

#endif  // ! SCHEDULER_H
