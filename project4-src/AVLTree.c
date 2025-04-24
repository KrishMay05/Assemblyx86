//
// C program to insert a node in AVL tree
//
// AVL code adapted from
//
// https://www.geeksforgeeks.org/insertion-in-an-avl-tree/
//

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "AVLTree.h"

long 
avl_compareKeys(KEYTYPE key1, KEYTYPE key2) {
	return strcmp(key1, key2);
}

// Get height of node
long 
avl_height(struct AVLNode *node)
{
    if (node == NULL)
        return 0;
    return node->height;
}

// Get maximum height
long avl_maxHeight(long a, long b)
{
    return (a > b)? a : b;
}

// Allocates a new AVL node 
AVLNode* 
avl_newNode(KEYTYPE key, DATATYPE data)
{
    AVLNode* node = (AVLNode*)
                        malloc(sizeof(struct AVLNode));

    memcpy(node->key,key, MAXKEY);
    node->data   = data;
    node->left   = NULL;
    node->right  = NULL;
    node->height = 1;  // new node is initially added at leaf
    return(node);
}

// Rotate subtree rooted with y
AVLNode *
avl_rightRotate(struct AVLNode *y)
{
    struct AVLNode *x = y->left;
    struct AVLNode *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = avl_maxHeight(avl_height(y->left),
                    avl_height(y->right)) + 1;
    x->height = avl_maxHeight(avl_height(x->left),
                    avl_height(x->right)) + 1;

    // Return new root
    return x;
}

// A utility function to left rotate subtree rooted with x
// See the diagram given above.
AVLNode *
avl_leftRotate(AVLNode *x)
{
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    //  Update heights
    x->height = avl_maxHeight(avl_height(x->left),   
                    avl_height(x->right)) + 1;
    y->height = avl_maxHeight(avl_height(y->left),
                    avl_height(y->right)) + 1;

    // Return new root
    return y;
}

// Get Balance factor of node N
long 
avl_getBalance(struct AVLNode *N)
{
    if (N == NULL)
        return 0;
    return avl_height(N->left) - avl_height(N->right);
}

// Recursive function to insert a key in the subtree rooted
// with node and returns the new root of the subtree.
AVLNode* 
avl_insertNode(struct AVLNode* node, KEYTYPE key, DATATYPE data)
{
    /* 1.  Perform the normal BST insertion */
    if (node == NULL)
        return(avl_newNode(key, data));

    long result = avl_compareKeys(key, node->key);

    if (result < 0)
        node->left  = avl_insertNode(node->left, key, data);
    else if (result > 0)
        node->right = avl_insertNode(node->right, key, data);
    else // Equal keys are not allowed in BST
        return node;

    /* 2. Update height of this ancestor node */
    node->height = 1 + avl_maxHeight(avl_height(node->left),
                        avl_height(node->right));

    /* 3. Get the balance factor of this ancestor
          node to check whether this node became
          unbalanced */
    long balance = avl_getBalance(node);

    // If this node becomes unbalanced, then
    // there are 4 cases

    // Left Left Case
    if (balance > 1 && avl_compareKeys(key,node->left->key) < 0)
        return avl_rightRotate(node);

    // Right Right Case
    if (balance < -1 && avl_compareKeys(key, node->right->key) > 0)
        return avl_leftRotate(node);

    // Left Right Case
    if (balance > 1 && avl_compareKeys(key, node->left->key) > 0)
    {
        node->left =  avl_leftRotate(node->left);
        return avl_rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && avl_compareKeys(key,node->right->key) < 0)
    {
        node->right = avl_rightRotate(node->right);
        return avl_leftRotate(node);
    }

    /* return the (unchanged) node pointer */
    return node;
}

// A utility function to print preorder traversal
// of the tree.
// The function also prints height of every node
void 
avl_preOrder(struct AVLNode *root)
{
    if(root != NULL)
    {
	printf("%ld: %-31s %16p %16ld\n", root->height, root->key, root->data, (long)root->data);
        avl_preOrder(root->left);
        avl_preOrder(root->right);
    }
}

/*
int main()
{
  struct AVLTree btree;

  avl_initialize(&btree);

  avl_insert(&btree, "George", (void*) 10L);
  avl_insert(&btree, "Mary", (void*) 20L);
  avl_insert(&btree, "Peter", (void*) 30L);
  avl_insert(&btree, "Paul", (void*) 40L);
  avl_insert(&btree, "Anne", (void*) 50L);
  avl_insert(&btree, "John", (void*) 25L);

  printf("Preorder traversal : \n");
  avl_preOrder(btree.root);
  printf("\n");

  DATATYPE *data = avl_find_recursive(&btree, "George");
  printf("George -> %ld\n", (unsigned long) data);

  data = avl_find_iterative(&btree, "Paul");
  printf("Paul -> %ld\n", (unsigned long) data);

  return 0;
}
*/
