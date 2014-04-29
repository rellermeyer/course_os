/******************************************************
 *      queue.h
 *      
 *	Authors: Kevin Hicks, Anthony Lopez
 *
 *	Date: 24 April 2014
 *
 *	Purpose: Get in line.
 * 	(provide a queue data structure for CourseOS)
 *	function prototypes
 *
 ******************************************************/

#include <stdlib.h>

#ifndef __queue_h
#define __queue_h

#define MAX_SIZE 100

typedef struct queue_elem queue_elem;
typedef struct queue queue;

struct queue_elem {
  void *data;
};

typedef struct queue {
  queue_elem data[MAX_SIZE];
  int front;
  int rear;
};

queue* create_queue(void *data);
int is_empty(queue* queue);
int is_full(queue* queue);
int size(queue* queue);
queue_elem peek(queue* queue);
void enqueue(queue* queue, queue_elem* elem);
queue_elem dequeue(queue* queue);



#endif
