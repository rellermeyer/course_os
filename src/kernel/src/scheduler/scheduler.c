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

int idCounter;

void init_scheduler() {
    kprintf("setting the scheduler periodic interupt\n");
    chipset.schedule_timer_periodic(&schedulerTimerCallback, TIME_SLICE_MS);
}

void schedulerTimerCallback() {
    getNext();
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

void getNext() {
    timer += TIME_SLICE_MS;

    for (ProcessControlBlock * i = sleepQueue; i != NULL && i->wakeupTime <= timer; i = i->next) {
        removePCBNode(i);
        add(i, false);
    }

    if (queue == NULL) {
        FATAL("(no processes active)");
    }
    queue = queue->next;
    // Put queue->execution state in r4
    asm volatile("MOV %0, r4" : "=r"(queue->executionState));
    // Put l1page/vas in r5
    // TODO change l1page to vas if necessary
    asm volatile("MOV %0, r5" : "=r"(queue->executionState->l1_page_table));
}
