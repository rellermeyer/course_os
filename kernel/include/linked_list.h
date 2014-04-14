/********************************************************************
*	linked_list.h
*
*	Author: Brandon Olivier	// any collaborators, please add name
*
*	Date: 28 March 2014
*
*	Purpose:	Provide basic data structures for CourseOS
*				This header provides function skeletons
*				for structures.c
*
********************************************************************/

#ifndef __llist_h
#define __llist_h

typedef struct {
    struct ll_node *next;
    void *data;
} ll_node;

typedef struct {
    ll_node *head;
    int size;
} list;

/* prepend (because of speed) to the list. Return 0 if fails */
ll_node* create(void *data);
void free_list(list *l);
int prepend(list *l, void *data); 
int insert(list *l, void *data, int index);
int delete_at(list *l, int index); 
void* get(list *l, int index);
int size(list *l);
int set_data(list *l, void *data);
int resize(list *l);

#endif
