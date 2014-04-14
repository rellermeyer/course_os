/********************************************************************
*	linked_list.c
*
*	Author: Brandon Olivier	// any collaborators, please add name
*
*	Date: 28 March 2014
*
*	Purpose: Provide basic linked list for CourseOS
*                Implementation file
*
********************************************************************/

#include "linked_list.h"

list* create(void *data){
    list *result = malloc(20 * sizeof(ll_node));
    result.size = 0;
    result->head = NULL;
    return result;
}

/* since free isn't impmlemented, it's not going to do anything */
/* TODO: implmement free */
void free_list(list *l) {
    ll_node tmp = l->head;
    ll_node next = tmp->next;
    while(tmp->next) {
	free(tmp); 
	tmp = next;
	next = tmp->next;
    } 
    free(l); 
}
