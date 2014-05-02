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
    int left_balance;
    int right_balance;
};

tree* init_tree();
node* init_node(void *d);
node* traverse(tree *t, (void *)addr);
void insert(tree* t, void *d);
void delete(tree *t, node *n);
void rotate_if_needed(tree *t);
int greater_than(void *a, void *b);
int balance_factor(node *n);

#endif
