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

 stack_elem* create_elem
