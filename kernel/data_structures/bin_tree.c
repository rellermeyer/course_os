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
#include "mem_alloc.h"
#include "bin_tree.h"


tree* init_tree()
{
    return (tree *) malloc(sizeof(tree));
}

int comparison(void *a, void *b)
{
    int x = *(int *)a;
    int y = *(int *)b;
    if (x < y)
        return -1;
    if (x > y)
        return 1;
    return 0;
}

void llist_insert(tree* t, void *d)
{
    node *next = t->root;
    while(next) {
        switch (comparison(next->address, d)) {
        case 1:
            next = next->right;
            break;
        case -1:
            next = next->left;
            break;
        case 0:
            return;
            break;
        default:
            return;
        }
    }
    
}
void delete(tree *t, node *n)
{
    return; 
}
