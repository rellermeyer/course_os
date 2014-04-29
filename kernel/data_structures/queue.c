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
  
}

int size(queue* queue) {
}

queue_elem peek(queue* queue) {
}

void push(queue* queue, queue_elem* elem) {
}

queue_elem pop(queue* queue) {
}
