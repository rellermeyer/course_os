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
    chipset.schedule_timer_periodic(void * /*fill with proper callback*/, TIME_SLICE_MS);
    // TODO how to deal with empty/initial queue???
}

void sleep(int id, int sleepTime) 
{
    ProcessControlBlock node = removePCBNode(id);
    
    node.wakeupTime = timer + min(MINSLEEPTIME, sleepTime);
    ProcessControlBlock * j = sleepQueue;
    while (j->wakeupTime <= node->wakeupTime && j->next == nullptr) { j = j->next }
    addPCBNodeAfter(j, node);
}

void remove(int id) {
    if (removePCBNode(id, sleepQueue) != nullptr) return;
    if (removePCBNode(id, queue) == nullptr) return;
    // TODO how to deal with empty/initial queue???
    // queue = new ProcessControlBlock {next = this, prev = this}
}

void * getNext() {
    timer += TIME_SLICE_MS;

    for (ProcessControlBlock* i = sleepQueue; i->wakeupTime <= timer; i = i->next) {
        addPCBNodeAfter(queue, removePCBNode(i));
    }

    // TODO how to deal with empty/initial queue???
    if (queue == nullptr) return nullptr;
    queue = queue->next;
    return queue;
}
