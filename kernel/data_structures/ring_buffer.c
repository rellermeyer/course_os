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
#include "../include/mem_alloc.h"
#include "../include/locks.h"

ring_buffer* create()
{
    ring_buffer* rb = (ring_buffer *) u_malloc(sizeof(ring_buffer));
    rb->size = 0;
    rb->l = 0;
    return rb;
}

void free_ring_buffer(ring_buffer *r)
{
	lock(r->l->lock);
    rb_node *clear = r->head;
    rb_node *cur = r->head->next;
    while(cur->next) {
        free(clear);
        clear = cur;
        cur = cur->next;
    }
    free(cur);
}

int put(ring_buffer *r, void *data)
{ /* do a size check */
    lock(r->l->lock);
    r->size++;
    rb_node* node = (rb_node *) u_malloc(sizeof(rb_node));
    node->data = data;
    r->head->next = node;
    r->head = node;
    node->next = r->tail;
    unlock(r->l->lock);
    return 1;
}

rb_node* get(ring_buffer *r)
{
    return r->tail;
}

void clear(ring_buffer *r)
{
	lock(r->l->lock);
    rb_node* clear = r->tail;
    r->tail = clear->next;
    r->head->next = r->tail;
	//free(clear->data); // nots sure if good idea
    free(clear);
	unlock(r->l->lock);
}
