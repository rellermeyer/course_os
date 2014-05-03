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

#include "../include/ring_buffer.h"

ring_buffer* create()
{
    ring_buffer* rb = (ring_buffer *) umalloc(sizeof(ring_buffer));
    rb->size = 0;
    rb->l = 0;
    return rb;
}

void free_ring_buffer(ring_buffer *r)
{
    rb_node *clear = r->head;
    rb_node *cur = r->head->next;
    while(cur->next) {
        ufree(clear);
        clear = cur;
        cur = cur->next;
    }
    ufree(cur);
}

int put(ring_buffer *r, void *data)
{ /* do a size check */
    r->size++;
    rb_node* node = (rb_node *) umalloc(sizeof(rb_node));
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
    ufree(clear);
}
