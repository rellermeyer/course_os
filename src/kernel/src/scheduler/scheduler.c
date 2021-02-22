#include <processControlBlock.h>
#include <scheduler.h>
#include <math.h>

#define TIME_SLICE_MS       10
#define MINSLEEPTIME        20

ProcessControlBlock * queue;
ProcessControlBlock * sleepQueue;

int timer = 0;

int idCounter;

void init_scheduler() {
    //chipset.schedule_timer_periodic(void * /*fill with proper callback*/, TIME_SLICE_MS);
}

void sleep(int id, int sleepTime) 
{
    ProcessControlBlock node = removePCBNode(id);
    
    node.wakeupTime = timer + min(MINSLEEPTIME, sleepTime);
    ProcessControlBlock * j = sleepQueue;
    while (j->wakeupTime <= node->wakeupTime && j->next == nullptr) { j = j->next }
    addPCBNodeAfter(j, node);
}

void add(ProcessControlBlock * new, bool front) {
    if (queue == nullptr) {
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
    if (removePCBNode(id, queue) != nullptr) return;
    removePCBNode(id, sleepQueue)
}

void * getNext() {
    timer += TIME_SLICE_MS;

    for (ProcessControlBlock* i = sleepQueue; i->wakeupTime <= timer; i = i->next) {
        add(removePCBNode(i), front);
    }

    // TODO This might break a lot of stuff
    if (queue == nullptr) return queue;
    queue = queue->next;
    return queue;
}
