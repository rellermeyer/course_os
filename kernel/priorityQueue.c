#include "include/priorityQueue.h"
#include "include/global_defs.h"

/* Global Variables */
Node *currentNode;
/********************/

/* Initializes all the global variables */
void init_q() {
    head = (Node *)kmalloc(sizeof(Node));
    os_printf("&head=%x\n", head);
    head->next = NULL;
    head->prev = NULL;
    head->PCB = NULL;
    head->priority = NOT_SET;
    head->PCB->current_state = NOT_SET;
    currentNode = head;
}

/* Add a PCB to the queue with a given priority.
   Return 1 if successful.
 */
int add(pcb *PCB, int priority) {

    Node *newTask = (Node *)kmalloc(sizeof(Node));
    os_printf("&newTask=%x\n", newTask);
    newTask->next = NULL;
    newTask->prev = NULL;
    newTask->PCB = PCB;
    newTask->priority = priority;
    PCB->current_state = PROCESS_READY;

    if(head->next == NULL) {
        head->next = (struct node *)newTask;
        newTask->next = (struct node *)head;
        newTask->prev = (struct node *)head;
        head->prev = (struct node *)newTask;
    }
    else {
        // Add the new task into the correct position based on priority
        currentNode = head;
        while((currentNode->next != (struct node *)head) &&
          (currentNode->next->priority > newTask->priority))
        {
            currentNode = (Node *)currentNode->next;
        }
        newTask->next = currentNode->next;
        newTask->prev = (struct node *)currentNode;
        currentNode->next->prev = newTask;
        currentNode->next = (struct node *)newTask;
    }

    return 1; // Change if there should be a condition for not adding a PCB to the priority queue.
}

/* Remove the node with the same PID as the process in the given process
   control block.

   Return a pointer to the PCB removed.
*/
pcb* remove(pcb *PCB) {
  //PCB abstraction will change the parameters, can search for processes by PID.
  currentNode = head;
  while(currentNode->next->PCB->PID != PCB->PID) {
    currentNode = (Node *)currentNode->next;
  }
  Node *nodeToRemove = (Node *)currentNode->next;
  currentNode->next = nodeToRemove->next;
  currentNode->next->prev = currentNode;
  nodeToRemove->next = nodeToRemove->prev = NULL;
  return nodeToRemove->PCB;
}

/* Wait for a task to finish. Then set the current task's state to READY */
void join(pcb *other_PCB) {
  // TODO: Do we need to store which task(s) we are blocked on?
  currentNode->PCB->current_state = PROCESS_BLOCKED;

  // Wait for the other thread to exit
  while (other_PCB->current_state != PROCESS_DYING ||
    is_in_queue(other_PCB->PID))
  {
    task_yield();
  }

  currentNode->PCB->current_state = PROCESS_RUNNING;
}

/* If the task has not been started yet, start it.
   Else if the task was previously started, restore its state.
*/
void dispatch(pcb *PCB) {
  os_printf("dispatch\n");
  if (PCB->current_state == PROCESS_NEW) // TODO: not sure if this is correct
  {
    PCB->current_state = PROCESS_RUNNING;
    execute_process(PCB); // Found in process.c
  }
  else
  {
    // This will jump to the location the state was saved in task_yield()
    //load_process_state(PCB->PID); // Found in process.c
    execute_process(PCB);
  }
}

/* Schedule a new task and call dispatch() to run it. */
void schedule() {
  // When dispatch() returns, we must schedule again, so we have use "while"
  while (head->next != NULL)
  {
    os_printf("schedule\n");
    Node *nodeToDispatch = (Node *)head->next;

    if (nodeToDispatch->PCB->current_state == PROCESS_READY)
    {
      nodeToDispatch->PCB->current_state = PROCESS_RUNNING;
      dispatch(nodeToDispatch->PCB);
    }
    else
    {
      // If the task isn't ready, schedule another.
      head = (Node *)head->next;
    }

    head = head->next;
  }

  // If we ever reach this part, we are out of things to schedule
}

/* I heard we wanted voluntary yielding to start. Tasks should call this
   function every so often.
*/
void task_yield()
{
  /* Save the state of the task that is currently running. We'll jump
     back to here later.
  */
  currentNode->PCB->current_state = PROCESS_READY;
  int has_jumped = save_process_state(currentNode->PCB->PID);

  if (has_jumped == FALSE)
  {
    // If the jump has NOT happened, schedule()
    schedule();
  }
  else
  {
    // If the jump has happened, return to the task
    currentNode->PCB->current_state = PROCESS_RUNNING;
    return;
  }
}

/* Return TRUE if a process with a PID of pid is in the queue.
   Return FALSE otherwise.
*/
Boolean is_in_queue(int pid)
{
  Node *node_ptr = head;

  // Iterate through the list
  while ((Node *)node_ptr->next != head)
  {
    if (node_ptr->PCB->PID == pid)
    {
      return TRUE;
    }

    node_ptr = (Node *)node_ptr->next;
  }

  return FALSE;
}
