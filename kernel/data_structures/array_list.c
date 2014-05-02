/********************************************************************
 *      linked_list.c
 *
 *      Author: Kevin Hicks // any collaborators, please add name
 *
 *      Date: 2 May 2014
 *
 *      Purpose: Provide basic linked list for CourseOS
 *                Implementation file
 *
 *       TODO: Use locks or atomic operations to ensure that
 *             functions are safe
 *
 *
 *
 *
 *
 ********************************************************************/

#include "../include/array_list.h"
#include "../include/mem_alloc.h"

a_list* create_array_list()
{
    a_list *result = (a_list *) malloc(sizeof(a_list));
    result->size = 0;
    return result;
}

al_node* create_al_node(void *data) {
    al_node *node = (al_node*) malloc(sizeof(al_node));
    node->data = data;
    return node;
}

void al_insert(a_list *l, void *data, int index)
{
    int i;
    l->size++;
    for(i = l->size-1; i >= index; i--) {
      l->data[i+1] = l->data[i];
    }
    l->data[index] = create_al_node(data);
}

void al_append(a_list *l, void *data)
{
    l->size++;
    l->data[l->size-1] = create_al_node(data);
}

void al_delete_at(a_list *l, int index) {
    int i;
    l->size--;
    for (i = index; i<l->size; i++) {
      l->data[i] = l->data[i+1];
    }
}

al_node* al_get(a_list *l, int index)
{
    return &l->data[index]; 
}

void al_replace(a_list *l, void *data, int index)
{
    l->data[index] = create_al_node(data);
}
