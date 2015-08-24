/********************************************************************
 *      ring_buffer.c
 *
 *      Author: Brandon Olivier // any collaborators, please add name
 *
 *      Date: 14 April 2014
 *
 *      Purpose:        Provide ring buffer implementation for CourseOS
 *
 *      TODO: Add locks or mutexes or something
 *
 ********************************************************************/

#include "ring_buffer.h"

ring_buffer* create(int size)
{
    ring_buffer* rb = (ring_buffer *) malloc(sizeof(ring_buffer) * size);
    rb->size_limit = size;
    return rb;
}

int put(ring_buffer *r, void *data)
{ /* do a size check */
    if(r->size >= r->size_limit)
	return 0; // no space
    r->size++;
    rb_node* node = (rb_node *) malloc(sizeof(rb_node));
    node->data = data;
    r->head->next = node;
    r->head = node;
    node->next = r->tail;
    return 1;
}

rb_node* get(ring_buffer *r)
{
    return r->tail;
}

void clear(ring_buffer *r)
{
    rb_node* clear = r->tail;
    r->tail = clear->next;
    r->head->next = r->tail;
    //free(clear->data); // nots sure if good idea
    free(clear);
}

ring_buffer* increase_size(ring_buffer *r, int by)
{
    int new_limit = r->size + by;
    ring_buffer *new = (ring_buffer *) malloc(sizeof(ring_buffer) * new_limit);
    new->size_limit = new_limit;
    new->size = r->size;
    new->tail = r->tail;
    new->head = r->head;
    free(r);
    return new;
}
