
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "AVLTree.h"
#include "tsearch.h"

DATATYPE
avl_find_recursive_node( TreeSearch * tsearch, AVLNode * node, KEYTYPE key) {

	if ( node == NULL) {
		// Not found
		return NULL;
	}

	long result = avl_compareKeys(key, node->key);

	if ( result > 0 ) {
		return avl_find_recursive_node(tsearch, node->right, key);
	}
	else if (result < 0 ) {
		return avl_find_recursive_node(tsearch, node->left, key);
	}
	else {
		return node->data;
	}
	
}

DATATYPE
tsearch_find(TreeSearch * tsearch, KEYTYPE key)
{
	return avl_find_recursive_node(tsearch, tsearch->root, key);
}

