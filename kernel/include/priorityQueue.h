#define NOT_SET -1

// Process statuses
#define READY 1
#define BLOCKED 2
#define RUNNING 3

/* Data Structure: Doubly Linked List */
typedef struct {
    struct node *next;
    struct node *prev;
    void *PCB; //Pointer PCB
    int priority;
    int status; // Blocked, running, ready, etc.
} Node;


void init();
int add(void *PCB, int priority);
void* remove(void *PCB);
void join(void* TCB);
void dispatch(void *PCB);
void schedule();
void task_yield();
