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
#include "../include/klibc.h"

llist_handle* llist_create() { /* create more space than needed -- less resizing */
	llist_handle *result = (llist_handle *) kmalloc(sizeof(llist_node));
	result->count = 0;
	return result;
}

int llist_count(llist_handle *l) {
	return l->count;
}

void* llist_get_data(llist_handle *l, int index) {
	return llist_get_node(l, index)->data;
}

llist_node* llist_get_node(llist_handle *l, int index) {
	int i;
	llist_node *tmp = l->head;
	for (i = 0; i < index; i++) {
		tmp = tmp->next;
	}
	return tmp;
}

void llist_set_data(llist_node *l, void *data) {
	l->data = data;
}

llist_node* llist_create_node(void *data) {
	llist_node *node = (llist_node*) kmalloc(sizeof(llist_node));
	node->data = data;
	return node;
}

void llist_enqueue(llist_handle * list, void * data) {
	llist_insert(list, data, 0);
}

void llist_remove_at(llist_handle *l, int index) {
	llist_node *prev = llist_get_node(l, index);
	llist_node *to_delete = prev->next;
	prev->next = to_delete->next;
	l->count--;
	llist_free_node(to_delete);
}

void* llist_dequeue(llist_handle * list) {
	if (llist_count(list) > 0) {
		void * data = llist_get_data(list, 0);
		llist_remove_at(list, 0);
		return data;
	}
	return 0;
}

/* TODO: implmement free */
void llist_free(llist_handle *l) { /* since free isn't really implemented, it's not going to do anything */
	llist_node *tmp = l->head;
	llist_node *next = tmp->next;
	while (tmp->next) {
		kfree(tmp);
		tmp = next;
		next = tmp->next;
	}
	kfree(l);
}

void llist_free_node(llist_node *node) {
	kfree(node->data);
	kfree(node);
}

void llist_insert(llist_handle *l, void *data, int index) {
	llist_node *new_node = llist_create_node(data);

	if (index == 0) {
		new_node->next = l->head;
		l->head = new_node;
	} else {
		llist_node *next = l->head;
		llist_node *prev;
		for (int i = 0; i < index; i++) {
			prev = next;
			next = next->next;
		}

		prev->next = prev ? new_node : 0;
		next->next = next ? new_node : 0;
	}

	l->count++;
}
