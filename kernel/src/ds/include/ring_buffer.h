/********************************************************************
 *      ring_buffer.h
 *
 *      Author: Brandon Olivier // any collaborators, please add name
 *
 *      Date: 14 April 2014
 *
 *      Purpose:        Provide ring buffer for CourseOS
 *                              Data gets PUT in at the head index 
 *                              and it is GET at the tail index
 *
 ********************************************************************/
#include "mem_alloc.h"

typedef struct rb_node rb_node;
typedef struct ring_buffer ring_buffer;

struct rb_node
{
    rb_node *next;
    rb_node *prev;
    void *data;
};

struct ring_buffer
{
    rb_node *head;
    rb_node *tail;
    int size;
    int size_limit;
};
ring_buffer* create(int size);
void free_ring_buffer(ring_buffer *r);

int put(ring_buffer *r, void *data);
rb_node* get(ring_buffer *r);
void clear(ring_buffer *r);
ring_buffer* increase_size(ring_buffer *r, int by); // as in increase by x 
