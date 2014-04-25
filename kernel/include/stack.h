/******************************************************
 *      stack.h
 *      
 *	Authors: Kevin Hicks, Anthony Lopez
 *
 *	Date: 22 April 2014
 *
 *	Purpose: Stack it up, baby.
 * 	(provide a stack data structure for CourseOS)
 *	function prototypes
 *
 ******************************************************/

#ifndef __stack_h
#define __stack_h

#define MAX_SIZE 100

typedef struct stack_elem stack_elem;
typedef struct stack stack;

struct stack_elem {
  void *data;
}

typedef struct stack {
  stack_elem data[MAX_SIZE];
  int ptr;
}

stack* create_stack(void *data);
int is_empty(stack* stack);
int size(stack* stack);
stack_elem peek(stack* stack);
void push(stack* stack, stack_elem* elem);
stack_elem pop(stack* stack);



#endif
