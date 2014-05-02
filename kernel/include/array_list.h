/********************************************************************
 *      linked_list.h
 *
 *      Author: Kevin Hicks // any collaborators, please add name
 *
 *      Date: 2 May 2014
 *
 *      Purpose:        Provide array lists for CourseOS
 *                              This header provides function skeletons
 *                              for array_list.c
 *
 ********************************************************************/

#ifndef __llist_h
#define __llist_h

#include "../include/locks.h"

#define START_SIZE 100

typedef struct al_node al_node;
typedef struct a_list a_list;


struct al_node
{
    void *data;
};
struct a_list
{
    al_node data[START_SIZE];
    int size;
};

a_list* create_array_list();
al_node* create_al_node(void *data);
void al_insert(a_list *l, void *data, int index);
void al_append(a_list *l, void *data);
void al_delete_at(a_list *l, int index);
al_node* al_get(a_list *l, int index);
void al_replace(a_list *l, void *data, int index);

#endif
