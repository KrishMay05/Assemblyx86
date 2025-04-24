
#include "AVLTree.h"

typedef struct TreeSearch 
{
    AVLNode * root;
} TreeSearch;

void tsearch_init(TreeSearch * tsearch);
void tsearch_print(TreeSearch * tsearch);
void tsearch_insert(TreeSearch * tsearch, KEYTYPE key, DATATYPE data);
DATATYPE tsearch_find(TreeSearch * tsearch, KEYTYPE key);

