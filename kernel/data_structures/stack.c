/********************************************************************
 *      stack.c
 *
 *      Authors: Kevin Hicks, Anthony Lopez // any collaborators, please add name
 *
 *      Date: 22 April 2014
 *
 *      Purpose: Provide basic stack for CourseOS
 *                Implementation file
 *
 *
 *
 *
 *
 *******************************************************************/

 #include "../include/stack.h"
 #include "../include/mem_alloc.h"

stack* create_stack() {
  stack *result = (stack *) malloc(sizeof(stack_elem));
  result->front = -1;
  return result;
}

stack_elem* create_stack_node(void *data) {
  stack_elem *elem = (stack_elem*) malloc(sizeof(stack_elem));
  elem->data = data;
  return elem;
}

int is_empty(stack* stack) {
  if (stack->ptr == -1)
    return 1;
  return 0;
  // Who needs booleans?
}

int size(stack* stack) {
  return stack->ptr+1;
}

stack_elem peek(stack* stack) {
  if (!is_empty(stack))
    return stack->data[stack->ptr];
  return NULL;
}

void push(stack* stack, stack_elem* elem) {
  stack->data[++stack->ptr] = elem;
}

stack_elem pop(stack* stack) {
  if (!is_empty(stack))
    return stack->data[stack->ptr--];
  return NULL;
}
