
//
// C program to insert a node in AVL tree
//

#define MAXKEY 32

typedef char * KEYTYPE;
typedef void * DATATYPE; 

// An AVL tree node
typedef struct AVLNode
{
    char key[MAXKEY];
    DATATYPE data;
    struct AVLNode *left;
    struct AVLNode *right;
    long height;
} AVLNode;

long avl_compareKeys(KEYTYPE key1, KEYTYPE key2);
long avl_height(struct AVLNode *node);
long avl_maxHeight(long a, long b);
AVLNode* avl_newNode(KEYTYPE key, DATATYPE data);
AVLNode * avl_rightRotate(struct AVLNode *y);
AVLNode * avl_leftRotate(AVLNode *x);
long avl_getBalance(struct AVLNode *N);
AVLNode* avl_insertNode(struct AVLNode* node, KEYTYPE key, DATATYPE data);
void avl_preOrder(struct AVLNode *root);

