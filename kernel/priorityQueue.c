#include "include/priorityQueue.h"
#include "include/global_defs.h"

/* Global Variables */
Node *head;
Node *currentNode;
/********************/

/* Initializes all the global variables */
void init() {
    head = (Node *)mem_alloc(sizeof(Node));
    head->next = NULL;
    head->prev = NULL;
    head->PCB = NULL;
    head->priority = NOT_SET;
    head->status = NOT_SET;
    currentNode = head;
}

/* Add a PCB to the queue with a given priority.
   Return 1 if successful.
 */
int add(void *PCB, int priority) {
	if(head == NULL) {
		init();
	}

    Node *newTask = (Node *)mem_alloc(sizeof(Node));
    newTask->next = NULL;
    newTask->prev = NULL;
    newTask->PCB = PCB;
    newTask->priority = priority;
    newTask->priority = READY;

    if(head->next == NULL) {
        head->next = newTask;
        newTask->next = head;
        newTask->prev = head;
        head->prev = newTask;
    }
    else {
        currentNode = head;
        while(currentNode->next != head && currentNode->next->priority > newTask->priority) {
            currentNode = currentNode->next;
        }
        newTask->next = currentNode->next;
        newTask->prev = currentNode;
        currentNode->next->prev = newTask;
        currentNode->next = newTask;
    }

    return 1; // Change if there should be a condition for not adding a PCB to the priority queue.
}

/* Remove the node with the same PID as the process in the given process
   control block.

   Return a pointer to the PCB removed.
*/
void* remove(void *PCB) { //PCB abstraction will change the parameters, can search for processes by PID.
	currentNode = head;
	while(currentNode->next->PCB->PID != PCB->PID) {
		currentNode = currentNode->next;
	}
	Node *nodeToRemove = currentNode->next;
	currentNode->next = nodeToRemove->next;
	currentNode->next->prev = currentNode;
	nodeToRemove->next = nodeToRemove->prev = NULL;
	return nodeToRemove->PCB;
}

void join(void* TCB) { //future implementation for threads.
}

void dispatch(void *PCB) { //requires free-standing ASM code
  /* If the task has not been started yet, start it.
     To start a task, we need to call its main() function.

     Else if the task was previously started, restore its state.
     State-saving should be done in yield() or ulibc's proc_yield() (not sure)
  */


}

/* Schedule a new task and call dispatch() to run it. */
void schedule() {
  // When dispatch() returns, we must schedule again, so we have use "while"
  while (head->next != NULL)
  {
    head->status = READY; // TODO: implement task blocking
    Node *nodeToDispatch = head->next;

    if (nodeToDispatch->status = READY)
    {
      remove(nodeToDispatch);
      nodeToDispatch->status = RUNNING;
      dispatch(nodeToDispatch);
    }
    else
    {
      // If the task isn't ready, schedule another.
      head = head->next;
    }
  }

  // If we ever reach this part, we are out of things to schedule
}

/* I heard we wanted voluntary yielding to start. Tasks should call this
   function every so often.
*/
void task_yield()
{
  /* TODO: Save the state of the task that is currently running. We'll jump
     back to here later.

     Jeffrey Tang is working on a state-saving function in the ulibc branch.
     These arguments may not be correct
  */
  has_jumped = proc_yield(currentNode->PCB); // TODO: implement state-saving

  if (has_jumped == FALSE)
  {
    // If the jump has NOT happened, schedule()
    schedule();
  }
  else
  {
    // If the jump has happened, return to the task
    return;
  }
}
