/********************************************************************
 *      queue.c
 *
 *      Authors: Kevin Hicks, Anthony Lopez // any collaborators, please add name
 *
 *      Date: 28 April 2014
 *
 *      Purpose: Provide basic queue for CourseOS
 *                Implementation file
 *
 *
 *
 *
 *
 *******************************************************************/

 #include "../include/queue.h"
 #include "../include/mem_alloc.h"

 queue* create_queue(void *data) {
   queue *result = (queue *) malloc(sizeof(queue_elem));
   result->front = -1;
   result->rear = -1;
   return result;
 }

int is_empty(queue* queue) {
  return queue->front == -1;
}

int is_full(queue* queue) {
  return (queue->rear == MAX_SIZE - 1 && queue->front == 0) || queue->rear == queue->front - 1;
}

int size(queue* queue) {
  return 0;
}

queue_elem peek(queue* queue) {
  return queue->data[queue->front];
}

void enqueue(queue* queue, queue_elem elem) {
  if (is_full(queue))
    return;

  if (queue->front == -1) {
    queue->front++;
    queue->rear++;
  }
  else if (queue->rear == MAX_SIZE-1) {
    queue->rear = 0;
  }
  else {
    queue->rear++;
  }
  queue->data[queue->rear] = elem;
}

queue_elem* dequeue(queue* queue) {
  if (is_empty(queue))
    return NULL;

  queue_elem temp = queue->data[queue->front];
  
  if (queue->front == queue->rear) {
    queue->front = -1;
    queue->rear = -1;
  }
  else if (queue->front == MAX_SIZE-1) {
    queue->front = 0;
  }
  else {
    queue->front++;
  }

  return &temp;
}
