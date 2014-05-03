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

#include "../include/locks.h"

typedef struct ll_node ll_node;
typedef struct list list;


struct ll_node
{
    ll_node *next;
    void *data;
};
struct list
{
	struct lock *l;
    ll_node *head;
    ll_node *tail;
    int size;
};

list* empty_create_list();
list* create_list(void *data);
ll_node* create_node(void *data);
void free_list(list *l);
void free_node(ll_node *node);
void insert(list *l, void *data, int index);
void append(list *l, void *data);
void delete_at(list *l, int index);
void* get_data(list *l, int index);
ll_node* get_node(list *l, int index);
void set_data(ll_node *l, void *data);

#endif
