#ifndef COURSE_OS_PROCESS_H
#define COURSE_OS_PROCESS_H

#include <vp_array_list.h>
#include <vm.h>

#define MAX_PROCESS_PRIORITY 20
#define DEFAULT_PROCESS_PRIORITY 10

typedef struct Process {
    uint8_t priority;
    size_t pid;
    uint8_t exit_code;
    struct vas* vas;
    VPArrayList *threads;
    struct Process *parent;
} Process;

Process *create_process(void *entry, Process *parent);
void free_process(Process *process);

#endif //COURSE_OS_PROCESS_H
