#ifndef PROCESS_H
#define PROCESS_H
#include "global_defs.h"
#include <stdint.h>
#include "mem_alloc.h"

/*LOG:
3/15: Initial skeleton and comments by Josh Guan.
3/21: More research and comments by Faseeh Akhter and Josh Guan
3/31: Further research and beginning implementation of process initialization and creation by Faseeh Akhter, Taylor Smith, Sean Villars
4/2:  Fixed mem_alloc and began initial pcb creation by Sean Villars, Faseeh Akhter, Josh Guan, Taylor Smith

/*******************
a work in progress
memory boundaries?
********************

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

/* what are the bare essentials for a PCB

*/
typedef
enum PROCESS_STATE {PROCESS_NEW, PROCESS_READY, PROCESS_RUNNING, PROCESS_BLOCKED, PROCESS_DYING}
PROCESS_STATE;

static uint32_t MAX_PROCESSES = 32;
static uint32_t GLOBAL_PID;

typedef struct pcb{
//ID data
  char* name; /* for debugging purposes */
  uint32_t PID;
  uint32_t starting_address;
  //uint32_t process_number; // is this a mapping to actual executable image? or does it describe total number of processes?
  //uint32_t user_id;
  //uint32_t group_id;
  //uint32_t parent_id;

//CPU state data
  PROCESS_STATE current_state;

// WE ARE GOING TO TRY TO IMPLEMENT SETJMP/LONGJMP INSTEAD OF MANUALLY DEALING WITH THESE VALUES
// uint32_t PC;
// uint32_t SP;
  // 37 REGISTERS IN TOTAL: 31 GPRs, 6 SRs
// uint32_t CPSR; //current prog status register
// uint32_t SPSR; //saved prog status register when execption occurs
  //unbanked register
// uint32_t R0;
// uint32_t R1;
// uint32_t R2;
 // uint32_t R3;
// uint32_t R4;
// uint32_t R5;
// uint32_t R6;
// uint32_t R7;
  //banked registers
// uint32_t R8;
// uint32_t R9;
// uint32_t R10;
// uint32_t R11;
// uint32_t R12;
// uint32_t R13; //corresponds to the SP; do we need both?
// uint32_t R14;
// uint32_t R15; //corresponds to the PC; do we need both?

//Control data
  //int priority_value;
  //uint32_t elapsed_time;
  //uint32_t EFLAG;
  //uint32_t* process_relations
  //uint32_t total_cpu_time;

} pcb;

uint32_t* pcb_table; //Table showing all initialized processes.

uint32_t * next_free_slot_in_pcb_table();


/* interface
processes can
be made (initialized)
be terminated
be duplicated
be suspended
be resumed
*/

//input
//1) address of where the program code is in memory; elf and loader

//process
//do we n eed to create the stack and the heap?
//do we need to know where the data is?

//output
//create a corresponding pcb ds

int init_all_processes();
int process_create(uint32_t starting_address, char* process_name);

// static void process_exit(process p); //harder because we have to clean up
// int fork();
// int process_suspend(process p);
// int process_resume(process p);
#endif