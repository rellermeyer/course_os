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

typedef struct llist_node llist_node;

struct llist_node {
    llist_node *next;
    void *data;
};

typedef struct {
    llist_node *head;
    llist_node *tail;
    int count;
} llist_handle;

/* prepend (because of speed) to the list. */
llist_handle* llist_create(void *data);
llist_node* create_node(void *data);
void llist_free(llist_handle *l);
void llist_free_node(llist_node *node);
void llist_insert(llist_handle *l, void *data, int index);
void llist_enqueue(llist_handle *l, void *data);
void* llist_dequeue(llist_handle *l);
void llist_remove_at(llist_handle *l, int index);
void* llist_get_by_index(llist_handle *l, int index);
llist_node* llist_get_node(llist_handle *l, int index);
void llist_set_data(llist_node *l, void *data);
int llist_count(llist_handle *l);

#endif
