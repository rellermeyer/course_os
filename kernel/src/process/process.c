#include <process.h>
#include <stdlib.h>
#include <mem_alloc.h>
#include <vm.h>
#include <string.h>
//#include <file.h>
#include <bitvector.h>

static pcb** pcb_table;
static bit_vector* pcb_map;


pcb* __process_create();
void __process_elf_init(pcb* pcb_p, const char* name);
void __process_stack_init(pcb* pcb_p);
void __process_heap_init(pcb* pcb_p);

void process_init()
{
  pcb_table = kmalloc(MAX_PROCESSES * sizeof(pcb*));
  pcb_map = make_vector(MAX_PROCESSES);
}

pcb* process_create(const char *name)
{
	  pcb* pcb_p = __process_create();
	  if(!pcb_p) {
	    return NULL;
	  }

	  __process_elf_init(pcb_p, name);
	  __process_stack_init(pcb_p);
	  __process_heap_init(pcb_p);
	  return pcb_p;
}

int process_execute(pcb* pcb_p)
{
  //Copy the current process's program counter to the new process's return register
  //The new process will use R14 to return to the parent function
  asm("MOV %0, r15":"=r"(pcb_p->R14)::);

  pcb_p->current_state = PROCESS_RUNNING;

  vm_enable_vas(pcb_p->stored_vas);
  process_load_state(pcb_p);
  return 0;
}

int process_destroy(pcb* pcb_p){
  bv_lower(pcb_p->PID, pcb_map);
  kfree(pcb_p);
  return 0;
}

/*Spring 2015 course_os: Sathya Sankaran, Rakan Stanbouly, Jason Sim

 creates a process and initializes the PCB
 @param file pointer to location in memory of file
 @return pcb pointer upon success
 @return 0 if there is no more room in pcb table */
pcb* __process_create()
{
  int32_t pcb_index = bv_firstFree(pcb_map);
  if (pcb_index < 0) {
    return NULL;
  }
  pcb* pcb_p = (pcb*) kmalloc(sizeof(pcb));
  if (!pcb_p) {
    return NULL;
  }
  //initialize PCB
  pcb_p->stored_vas = vm_new_vas();
  pcb_p->PID = pcb_index;
  //4-13-15: function pointer should point to common() of file pointer.
  //         TODO: Eventually should be able to pass parameters. Put them on the stack (argv/argc)
  pcb_p->current_state = PROCESS_NEW;
  pcb_p->has_executed = 0;

  pcb_table[pcb_index] = pcb_p;
  bv_set(pcb_index, pcb_map);
  return pcb_p;
}

void __process_elf_init(pcb* pcb_p, const char* name) {
    // TODO: depends on fs. did it not before? how did that work? fix fs? etc
//  int fd = kopen(name, 'r');
//  uint32_t start = PROC_LOCATION;
//  uint32_t len = 0;
//
//  struct stats fstats;
//  get_stats(name, &fstats);
//  len = fstats.size;
//  os_printf("LOADING PROCESS <<%s>>, start address %X\n",
//            name, start, len);
//
//  for (int i = 0; i < (len / BLOCK_SIZE) + 1; i++)
//  {
//    uint32_t *v = (uint32_t*) (start + (i * BLOCK_SIZE));
//    int x = vm_allocate_page(pcb_p->stored_vas, (void*) v, VM_PERM_USER_RW);
//    assert(x == 0);
//    vm_map_shared_memory(KERNEL_VAS, (void*) v,
//                         pcb_p->stored_vas,(void*) v,
//                         VM_PERM_USER_RW);
//  }
//
//  int* location = (int*) start;
//  int counter = 0;
//  while (counter < len)
//  {
//    kread(fd, location, 4);
//    location += 1;
//    counter += 4;
//  }
//
//  Elf_Ehdr* success = (Elf_Ehdr*) load_file(pcb_p, (uint32_t*) start);
//  pcb_p->R15 = success->e_entry;
//  for (int i = 0; i < (len / BLOCK_SIZE) + 1; i++)
//  {
//    uint32_t *v = (uint32_t *) (start + (i * BLOCK_SIZE));
//    vm_free_mapping(KERNEL_VAS, (void*) v);
//  }
}

/*
 Allocated memory for the process stack
 Moves arguments for argc, argv, envp, and auxp
 into stack_top

 Points stack pointer to location where stack_top would begin
 @param pointer to process control block
 @param pcb* pcb_p

 */
void __process_stack_init(pcb * pcb_p) {
  int retval = 0;
  for (int i = 0; i < (STACK_SIZE / BLOCK_SIZE); i++)
  {
    retval = vm_allocate_page(pcb_p->stored_vas,
        (void*) (STACK_BASE + (i * BLOCK_SIZE)), VM_PERM_USER_RW);
    if (retval)
    {
      os_printf("vm_allocate_page error code: %d\n", retval);
      break;
    }
    else
    {
      os_printf(
          "A page have been allocated for process stack at vptr: 0x%x\n",
          (STACK_BASE + (i * BLOCK_SIZE)));
    }
    vm_map_shared_memory(KERNEL_VAS,
                        (void*) (STACK_BASE + (i * BLOCK_SIZE)), pcb_p->stored_vas,
                        (void*) (STACK_BASE + (i * BLOCK_SIZE)), VM_PERM_USER_RW);
  }

  // Stick a NULL at STACK_TOP-sizeof(int*)
  uint32_t *stack_top = (uint32_t*) STACK_TOP;
  stack_top[-1] = 0;
  stack_top[-2] = 0;
  stack_top[-3] = 0;
  stack_top[-4] = 0;
  stack_top[-5] = STACK_BASE;
  stack_top[-6] = 1;

  strcpy((char*) STACK_BASE, pcb_p->name);

  // We need to set sp (r13) to stack_top - 12
  pcb_p->R13 = STACK_TOP - 4 * 6;
  for (int i = 0; i < (STACK_SIZE / BLOCK_SIZE); i++)
  {
    vm_free_mapping(KERNEL_VAS, (void*) (STACK_BASE + (i * BLOCK_SIZE)));
  }
}

void __process_heap_init(pcb* pcb_p) {
  //from mem_alloc.c
  init_process_heap(pcb_p->stored_vas);
  os_printf("User Level Heap for Process PID %d initialized\n", pcb_p->PID);
}

/*
 Saves all of the Registers on the machine to the PCB
 @param Process ID
 @return 0 if failed
 @return 1 for success
 */
void process_save_state(pcb* pcb_p) {
  assert(pcb_p);

  asm("MOV %0, r0":"=r"(pcb_p->R0)::);
  asm("MOV %0, r1":"=r"(pcb_p->R1)::);
  asm("MOV %0, r2":"=r"(pcb_p->R2)::);
  asm("MOV %0, r3":"=r"(pcb_p->R3)::);
  asm("MOV %0, r4":"=r"(pcb_p->R4)::);
  asm("MOV %0, r5":"=r"(pcb_p->R5)::);
  asm("MOV %0, r6":"=r"(pcb_p->R6)::);
  asm("MOV %0, r7":"=r"(pcb_p->R7)::);
  asm("MOV %0, r8":"=r"(pcb_p->R8)::);
  asm("MOV %0, r9":"=r"(pcb_p->R9)::);
  asm("MOV %0, r10":"=r"(pcb_p->R10)::);
  asm("MOV %0, r11":"=r"(pcb_p->R11)::);
  asm("MOV %0, r12":"=r"(pcb_p->R12)::);
  asm("MOV %0, r13":"=r"(pcb_p->R13)::);
  asm("MOV %0, r14":"=r"(pcb_p->R14)::);
  asm("MOV %0, r15":"=r"(pcb_p->R15)::);
}

#define offsetof(st, m) __builtin_offsetof(st, m)

__attribute((naked)) void process_load_state(pcb* pcb_p)
{
   asm volatile(
    "mov  ip, r0  \n\t" \
    "ldr  r0, [ip, %0] \n\t " \
	"ldr  r1, [ip, %1] \n\t " \
	"ldr  r2, [ip, %3] \n\t " \
	"ldr  r4, [ip, %4] \n\t " \
	"ldr  r5, [ip, %5] \n\t " \
	"ldr  r6, [ip, %6] \n\t " \
	"ldr  r7, [ip, %7] \n\t " \
	"ldr  r8, [ip, %8] \n\t " \
	"ldr  r9, [ip, %9] \n\t " \
	"ldr  r10, [ip, %10] \n\t " \
	"ldr  r11, [ip, %11] \n\t " \
	"ldr  r13, [ip, %12] \n\t " \
	"ldr  r14, [ip, %13] \n\t " \
	"ldr  ip, [ip, %14] \n\t " \
	"mcr p15, 0, r0, c8, c7, 0 \n\t" \
	"mov  r15, ip \n\t"
	:: "i" (offsetof(pcb, R0)),
	  "i" (offsetof(pcb, R1)),
	  "i" (offsetof(pcb, R2)),
	  "i" (offsetof(pcb, R3)),
	  "i" (offsetof(pcb, R4)),
	  "i" (offsetof(pcb, R5)),
	  "i" (offsetof(pcb, R6)),
	  "i" (offsetof(pcb, R7)),
	  "i" (offsetof(pcb, R8)),
	  "i" (offsetof(pcb, R9)),
	  "i" (offsetof(pcb, R10)),
	  "i" (offsetof(pcb, R11)),
	  "i" (offsetof(pcb, R13)),
	  "i" (offsetof(pcb, R14)),
	  "i" (offsetof(pcb, R15)) :
   );
}
/*
 Loads registers using values in pcb
 @param Process ID
 */
/*
void process_load_state(pcb* pcb_p)
{
  asm("MOV r0, %0"::"r"(pcb_p->R0):);
  asm("MOV r1, %0"::"r"(pcb_p->R1):);
  asm("MOV r2, %0"::"r"(pcb_p->R2):);
  asm("MOV r3, %0"::"r"(pcb_p->R3):);
  asm("MOV r4, %0"::"r"(pcb_p->R4):);
  asm("MOV r5, %0"::"r"(pcb_p->R5):);
  asm("MOV r6, %0"::"r"(pcb_p->R6):);
  asm("MOV r7, %0"::"r"(pcb_p->R7):);
  asm("MOV r8, %0"::"r"(pcb_p->R8):);
  asm("MOV r9, %0"::"r"(pcb_p->R9):);
  asm("MOV r10, %0"::"r"(pcb_p->R10):);
  asm("MOV r12, %0"::"r"(pcb_p->R12):);
  asm("MOV r13, %0"::"r"(pcb_p->R13):);
  asm("MOV r14, %0"::"r"(pcb_p->R14):);

  // move pc to process stack
  *((uint32_t*)(PROC_START+4)) = pcb_p->R15;

  vm_invalidate_tlb();

  asm("MOV r12, %0":: "i" ((PROC_START+4)):);
  asm("ldr r15, [r12]" :::);

  __builtin_unreachable();
}
*/
