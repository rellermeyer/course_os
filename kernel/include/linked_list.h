/********************************************************************
 *      linked_list.h
 *
 *      Author: Brandon Olivier // any collaborators, please add name
 *
 *      Date: 14 April 2014
 *
 *      Purpose:        Provide linked lists for CourseOS
 *                              This header provides function skeletons
 *                              for linked_list.c
 *
 ********************************************************************/

#ifndef __llist_h
#define __llist_h

#define NULL (void*) 0
typedef struct ll_node ll_node;
 struct ll_node
{
    ll_node *next;
    void *data;
};

typedef struct
{
    ll_node *head;
    ll_node *tail;
    int size;
} list;

/* prepend (because of speed) to the list. */
list* create_list(void *data);
ll_node* create_node(void *data);
void free_list(list *l);
void free_node(ll_node *node);
void insert(list *l, void *data, int index);
void delete_at(list *l, int index);
void* get_data(list *l, int index);
ll_node* get_node(list *l, int index);
void set_data(ll_node *l, void *data);

#endif
