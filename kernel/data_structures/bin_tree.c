/********************************************************************
 *      bin_tree.c
 *
 *      Author: Brandon Olivier // any collaborators, please add name
 *
 *      Date: 19 April 2014
 *
 *      Purpose: Implement b-trees for addresses
 *
 ********************************************************************/
#include "../include/mem_alloc.h"
#include "../include/bin_tree.h"


tree* init_tree()
{
    return (tree *) malloc(sizeof(tree));
    size = 0;
    root = (void *) 0; // NULL
}

node* init_node(void *a)
{
    node *r = (node *) malloc(sizof(node));
    left = (void *) 0;
    right = (void *) 0;
    left_balance = 0;
    right_balance = 0;
}


node* traverse(tree *t, (void *)addr)
{ // returns the parent of the future home of addr
    int test;
    cur = t->root;
    if(!cur){
	/* handle null case: insert, then return null */
	t->root = create_node(addr);
	return (void *) 0;
    }
    while(cur) {
	test = greater_than(addr, cur->data);
	if(test == 1)
	    if(cur->right)
		cur = cur->right;
	    else break;
	else if (test == 0)
	    if(cur->left)
		cur = cur->left;
	    else break;
	else
	    return (node *) 0; // found addr already
    }
    return cur;
}

int greater_than(void *a, void *b)
{ // better name than compare
 
    int x = *(int *)a;
    int y = *(int *)b;
    if (x < y)
        return 0;
    if (x > y)
        return 1;
    return 0; // default
}


void insert(tree* t, void *d)
{
    // actual inserting
    node *par = traverse(t, d);
    if(par && (greater_than(par->address, d))) {
	par->right = create_node(d);
    } else {
	par->left = create_node(d);
    } 
    node *cur = t->root;
    while(cur->address != d) {
	// update those balance factors
	if(greater_than(cur->address, d)) {
	    cur->right_balance++;
	    cur = cur->right;
	} else  {
	    cur->left_balance++;
	    cur = cur->left;
	} 
    }
    // any reshaping of tree
    reshape_if_needed(t, par);
}

void rehape(node *n)
{
    if(n->balance_factor() == 2) {
    if(n->left->balance_factor() == -1) // left right case
	left_right_shift(n);
    else(left_left_shift(n));
    } else { // right left case
	if(n->right->balance_factor == 1)
	    right_left_shift(n);
	else
	    right_right_shift(n);
    }
}
void reshape_if_needed(tree *t, node *cur)
{ 
    int test;
    while(cur != t->root) {
	test = cur.balance_factor();
	if(test > 1 || test < -1) {
	    reshape(cur);
	}
    }
}

void delete(tree *t, (void *) addr)
{
}

int balanceFactor(node *n)
{
    return n->left_balance - n->right_balance;
}

// boring shifting functions
// should be refactored into one or two functions
void left_right_shift(node *n)
{
    node *a = n->left;
    node *b = a->right;
    node *tmp = b->left;

    b->left = a;
    a->right = tmp;
}

void right_left_shift(node *n)
{
    node *a = n->right;
    node *b = a->left;
    node *tmp = b->right;

    b->right = a;
    a->left = tmp;
}

void left_left_shift(node *n) {
    node *a = n->left;
    node *b = a->left;
    node *tmp = a->right;

    a->right = n;
    n->right = tmp;
}

void right_right_shift(node *n)
{
    node *a = n->right;
    node *b = a->right;
    node *tmp = a->left;

    a->left = n;
    n->right = tmp;
} 
