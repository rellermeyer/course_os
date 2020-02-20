/********************************************************************
 *      bin_tree.h
 *
 *      Author: Brandon Olivier // any collaborators, please add name
 *
 *      Date: 19 April 2014
 *
 *      Purpose: Provide basis for b-trees for addresses
 *
 ********************************************************************/

#ifndef __b_tree_
#define __b_tree_

typedef struct tree tree;
typedef struct node node;

struct tree
{
    node *root;
    int size;
    int depth;
};

struct node
{
    node *left;
    node *right;
    void *address;
};

tree* init_tree();
void llist_insert(tree* t, void *d);
void delete(tree *t, node *n);

#endif
