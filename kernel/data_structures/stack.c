/********************************************************************
 *      linked_list.c
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

 #include "include/stack.h"
 #include "include/mem_alloc.h"

 stack* create_stack(void *data) {
   stack *result = (stack *) malloc(sizeof(stack_elem));
   result->size = 0;
   return result;
 }

int is_empty(stack* stack) {
  if (stack->size == -1)
    return 1;
  return 0;
  // Who needs booleans?
}

int size(stack* stack) {
  return stack->size+1;
}

stack_elem peek(stack* stack) {
  if (!is_empty(stack))
    return stack->data[stack->size];
  return NULL;
}

void push(stack* stack, stack_elem* elem) {
  stack->data[++stack->size] = elem;
}

stack_elem pop(stack* stack) {
  if (!is_empty(stack))
    return stack->data[stack->size--];
  return NULL;
}
