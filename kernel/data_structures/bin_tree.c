/********************************************************************
 *      bin_tree.c
 *
 *      Authors: Brandon Olivier, Collin Massey // any collaborators, please add name
 *
 *      Created: 19 April 2014
 *
 *      Last edit: 27 November 2015
 *
 *      Purpose: Implement unbalanced binary trees for addresses
 *
 ********************************************************************/
//#include "klibc.h"
//#include "data_structures/bin_tree.h"
#include <stddef.h>
#include <stdio.h>


typedef struct node {
   struct node *parent;
   struct node *left;
   struct node *right;
   void *data;
} node;

typedef struct tree {
   struct node *root;
   int size;
} tree;


struct tree *new_tree(node *root)
{
   struct tree *t = (tree *)kmalloc(sizeof(struct tree)); // TODO: Needs to be fixed
   t->root = root;
   t->size = 0;
   return t;
}

struct tree *init_tree()
{
   return new_tree(NULL);
}

// Compare 2 values
int comparison(void *a, void *b)
{
   int x = *(int *)a;
   int y = *(int *)b;
   if (x < y)
      return -1;
   if (x > y)
      return 1;
   return 0; // x = y (a = b)
}

// Insert a node into the tree if not already present
// Returns 1 if inserted, 0 otherwise
int insert(tree *t, void *val)
{
   if (contains_node(t, val)) {
      return 0;
   }

   struct node *new_node = (node *)kmalloc(sizeof(struct node)); // TODO: Needs to be fixed
   new_node->left = new_node->right = NULL;
   new_node->parent = NULL;
   new_node->data = val;

   if (t->root == NULL) { // Empty tree
      t->root = new_node;
      t->size += 1;
      return 1;
   }

   node *parent = NULL; // Will be parent of new node
   node *temp = t->root; // Temporary node
   int comp = 0; // Comparison value, used in final step

   // Traverse tree
   while(temp) {
      parent = temp;
      switch (comparison(temp->data, val)) {
      case 1:
         comp = 1;
         temp = temp->left;
         break;
      case -1:
         comp = -1;
         temp = temp->right;
         break;
      case 0: // This should never hit, but just in case
         return 0;
      default:
         return 0; // Undefined behavior
      }
   }

   // Add node to tree
   if (comp == 1) {
      parent->left = new_node;
   } else { // comp = -1
      parent->right = new_node;
   }

   new_node->parent = parent; // Each node know its parent
   t->size += 1;
   return 1;
}

// Returns the node that has data = val if present, NULL otherwise
node *find_node(tree *t, void *val)
{
   node *current_node = t->root;

   while (current_node) {
      switch (comparison(&current_node->data, val)) {
      case -1:
         current_node = current_node->right;
         break;
      case 1:
         current_node = current_node->left;
         break;
      case 0:
         return current_node;
      default:
         return NULL; // Undefined behavior
      }
   }

   // val not found
   return NULL;
}

// Returns 1 if the tree contains a node with data = val, 0 otherwise
int contains_node(tree *t, void *val)
{
   return find_node(t, val) != NULL ? 1 : 0;
}

// Returns the node with the minimum value in the tree provided
// If tree is empty, returns NULL
node *min_node(tree *t)
{
   node *temp = t->root;

   while (temp->left != NULL) {
      temp = temp->left;
   }

   return temp;
}

// Returns the minimum value in the tree, or 0 if not present
int min_val(tree *t)
{
   node *n = min_node(t);
   return n != NULL ? *((int *)n->data) : 0;
}

// Returns the node with the maximum value in the tree provided
// If tree is empty, returns NULL
node *max_node(tree *t)
{
   node *temp = t->root;

   while (temp->right != NULL) {
      temp = temp->right;
   }

   return temp;
}

// Returns the maximum value in the tree, or 0 if not present
int max_val(tree *t)
{
   node *n = max_node(t);
   return n != NULL ? *((int *)n->data) : 0;
}

// Remove node with data = val from tree if present
// Returns 1 if removed, 0 if node not present
int remove_node(tree *t, void *val)
{
   node *n = find_node(t, val);
   if (n == NULL) // node not present
      return 0;

   node *parent = n->parent;
   int children = 0; // # of children n has

   if (n->left != NULL)
      children += 1;
   if (n->right != NULL)
      children += 1;

   if (children == 0) { // Most simple
      if (parent->left == n) {
         parent->left = NULL;
      } else {
         parent->right = NULL;
      }

      kfree(n);
      t->size -= 1;
      return 1;
   } else if (children == 1) { // Less simple
      if (parent->left == n) {
         if (n->left != NULL) {
	    parent->left = n->left;
         } else {
	    parent->left = n->right;
	 }
      } else {
	 if (n->left != NULL) {
	    parent->right = n->left;
         } else {
	    parent->right = n->right;
         }
      }

      kfree(n);
      t->size -= 1;
      return 1;
   } else { // children = 2, least simple
      // Need to replace n's data with the smallest value in its right subtree
      tree *right_subtree = new_tree(n->right);
      node *min = min_node(right_subtree);
      node *min_parent = min->parent; // Parent of min node

      n->data = min->data; // Move min node to where n was

      // Erase duplicate
      if (min_parent->left == min) {
         min_parent->left = NULL;
      } else {
         min_parent->right = min->right;
      }

      kfree(min);
      t->size -= 1;
      return 1;
   }
}

// Support method for delete_tree
void delete(node *n)
{
   if (n != NULL) {
      delete(n->left);
      delete(n->right);
      kfree(n);
   }
}

// Remove all node pointers and the tree pointer
void delete_tree(tree *t)
{
   if (t->root != NULL)
      delete(t->root);
   
   kfree(t);
}

// Returns 1 if the tree is empty, 0 otherwise
int is_empty(tree *t)
{
   return t->root == NULL ? 1 : 0;
}

// Support method for print_tree
void print_nodes(node *n)
{
   printf("%d\n", *((int *)n->data));
   if (n->left != NULL)
      print_nodes(n->left);
   if (n->right != NULL)
      print_nodes(n->right);
}

// Print an in-order traversal of the tree
void print_tree(tree *t)
{
   node *root = t->root;
   if (root != NULL)
      print_nodes(root);
}
