/********************************************************************
 *      linked_list.c
 *
 *      Author: Brandon Olivier // any collaborators, please add name
 *
 *      Date: 14 April 2014
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

#include "../include/linked_list.h"
#include "../include/mem_alloc.h"

list* empty_create_list()
{
    list *result = (list *) u_malloc(sizeof(list));
    result->size = 0;
    result->head = 0;
    result->tail =  0;
    return result;
}
list* create_list(void *data)
{
    list *result = (list *) u_malloc(sizeof(list));
    result->head = 0;
    result->tail = 0;
    result->size = 0;
    insert(result, create_node(data), 0);
    return result;
}

ll_node* create_node(void *data) {
    ll_node *node = (ll_node*) u_malloc(sizeof(ll_node));
    node->data = data;
    return node;
}

void free_list(list *l)
{   /* since free isn't really implemented, it's not going to do anything */
    ll_node *tmp = l->head;
    ll_node *next = tmp->next;
    while(tmp->next) {
        free(tmp);
        tmp = next;
        next = tmp->next;
    }
    free(l);
}

void free_node(ll_node *node)
{
    free(node->data);
    free(node);
}

void insert(list *l, void *data, int index)
{
    int i;
    ll_node *next = l->head;
    ll_node *prev;
    l->size++;
    for(i = 0; i < index; i++) {
        prev = next;
        if(!next->next)
            return;
        next = next->next;
    }
    ll_node *new_node = create_node(data);
    prev->next = prev? new_node : (void *) 0; // null
    next->next = next? new_node : (void *) 0;
    while(l->tail->next) { // safer than if
        l->tail = l->tail->next;
    }
}

// this is what's broken
void append(list *l, void *data)
{
    l->tail->next = create_node(data);
    l->tail = l->tail->next;
}

void delete_at(list *l, int index) {
    ll_node *prev = get_node(l, index);
    ll_node *to_delete = prev->next;
    prev->next = to_delete->next;
    free_node(to_delete);
}

void* get_data(list *l, int index)
{
    return get_node(l, index)->data;
}

ll_node* get_node(list *l, int index)
{
    int i;
    ll_node *tmp = l->head;
    for(i = 0; i < index; i++) {
        tmp = tmp->next;
    }
    return tmp;
}

void set_data(ll_node *l, void *data)
{
    l->data = data;
}
