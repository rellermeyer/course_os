#include <processControlBlock.h>
#include <scheduler.h>
#include <math.h>
#include <stdio.h>
#include <interrupt.h>

#define TIME_SLICE_MS       10
#define MINSLEEPTIME        20

ProcessControlBlock * queue;
ProcessControlBlock * sleepQueue;

int timer = 0;

void init_scheduler(void) {
    kprintf("setting the scheduler periodic interupt\n");
    chipset.schedule_timer_periodic(&schedulerTimerCallback, TIME_SLICE_MS);
}

void schedulerTimerCallback(void) {
    timer += TIME_SLICE_MS;

    // Check if any sleeping processes can be awoken
    for (ProcessControlBlock * i = sleepQueue; i != NULL && i->wakeupTime <= timer; i = i->next) {
        removePCBNode(i);
        add(i, false);
    }
    // Save program state
    getNext();
    // Load program state
    //kprintf("schedulertimer interupt called");
}

void sleep(int id, int sleepTime)
{
    ProcessControlBlock * node = findNode(id, queue);
    removePCBNode(node);

    node->wakeupTime = timer + min(MINSLEEPTIME, sleepTime);
    ProcessControlBlock * j = sleepQueue;
    while (j->wakeupTime <= node->wakeupTime && j->next == NULL) { j = j->next; }
    addPCBNodeAfter(j, node);
}

void add(ProcessControlBlock * new, bool front) {
    if (queue == NULL) {
        queue = new;
        queue->next = queue;
        queue->prev = queue;
    } else {
        if (front)
            addPCBNodeAfter(queue, new);
        else
            addPCBNodeBefore(queue, new);
    }
}

void remove(int id) {
    if (removePCBNode(findNode(id, queue)) != NULL) return;
    removePCBNode(findNode(id, sleepQueue));
}

void getNext(void) {
    // Check if any possible processes
    if (queue == NULL) {
        FATAL("(no processes active)");
    }
    // Get next process
    queue = queue->next;
    switch_to_vas(queue->vas);
}

void schedule(void) {
    return getNext();
}
