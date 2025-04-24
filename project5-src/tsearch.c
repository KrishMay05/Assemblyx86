
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "tsearch.h"
#include <string.h>

void 
tsearch_init(TreeSearch * tsearch)
{
	tsearch->root = NULL;
}

void 
tsearch_print(TreeSearch * tsearch) {
	printf("  --------- TSEARCH DICTIONARY -------------\n");
	printf("Preorder traversal : \n");
  	avl_preOrder(tsearch->root);
  	printf("\n");
}

void 
tsearch_insert(TreeSearch * tsearch, KEYTYPE key, DATATYPE data)
{
        tsearch->root = avl_insertNode(tsearch->root, key, data);
}

