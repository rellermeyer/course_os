#ifndef PROCESS_H
#define PROCESS_H
#include "global_defs.h"
#include <stdint.h>

/* process struct outline
process id data
PID -- must be unique
User ID
Group ID
parent id ?

processor state data
pc
CPU GPRs
CPU status word
SP, FP

process control data
location of eimage on disk
location of user process kernel stack?


process scheduling state
priority value?
amount of time running or suspended
EFLAGs
process structuring information
process's child process ID
other related processes via their IDs
IPC info*
process privileges*
accounting info
last run?, total CPU time accumulation
*/

typedef struct process{
//ID data
  uint32_t PID;
  uint32_t user_id;
  uint32_t group_id;
  uint32_t parent_id;

//CPU state data
  uint32_t SPSR;
  uint32_t PC;

//Control data
  int priority_value;
  uint32_t elapsed_time;
  uint32_t EFLAG;
  uint32_t* process_relations

  uint32_t total_cpu_time;

} pcb;

/* interface
processes can
be made (initialized)
be terminated
be duplicated
be suspended
be resumed
*/

#endif
