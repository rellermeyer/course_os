#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <chipset.h>
#include <processControlBlock.h>
#include <interrupt.h>

void init_scheduler();
void schedulerTimerCallback();

void sleep(int id, int sleepTime);
void add(ProcessControlBlock * new, bool front);
void remove(int id);

void * getNext();  // should pick the next process to run and set the callback again
void * schedule(void * executionState); // interchange processes

#endif  // ! SCHEDULER_H
