#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <chipset.h>
#include <processControlBlock.h>
#include <interrupt.h>

void init_scheduler(void);
void schedulerTimerCallback(void);

void sleep(int id, int sleepTime);
void add(ProcessControlBlock * new, bool front);
void remove(int id);

void getNext(void);  // should pick the next process to run and set the callback again
void schedule(void); // interchange processes

#endif  // ! SCHEDULER_H
