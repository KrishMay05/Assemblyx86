
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tsearch.h"

DATATYPE 
tsearch_find(TreeSearch * tsearch, KEYTYPE key) {

	AVLNode * node = tsearch->root;
	
	while (node != NULL ) {
	
        	long result = avl_compareKeys(key, node->key);

        	if ( result > 0 ) {
                	node = node->right;
		}
        	else if (result < 0 ) {
                        node = node->left;
        	}
        	else {
                	return node->data;
		}
	}

	return NULL;
}

