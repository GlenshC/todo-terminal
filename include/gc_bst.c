#include "gc_bst.h"

#include <stdlib.h>

static GCBNode *gc_bst_node(void *value);

void gc_bst_init(GCBRoot *root)
{
    root->value = NULL;
    root->left = NULL;
    root->right = NULL;
}

void gc_bst_push(GCBRoot *root, void *value, bstCmpFun compare)
{
    if (root == NULL)
    {
        root = value;
        return;
    }
    
    // + when a is bigger (return > 0) store to right
    // - when b is bigger (return < 0) store to left
    // 0 when equal (return == 0) store to left
    GCBNode *node, *next;
    for (node = root; node != NULL; )
    {
        if (node->value == NULL)
        {
            node->value = value;
            break;
        }

        int result = compare(value, node->value);
        if (result > 0)
        {
            if (node->right == NULL)
            {
                node->right = gc_bst_node(value);
                break;
            }
            else
            {
                node = node->right;
            }
        }
        else
        {
            if (node->left == NULL)
            {
                node->left = gc_bst_node(value);
                break;
            }
            else
            {
                node = node->left;
            }
        }
    }
}

void gc_bst_display(GCBRoot *root, bstDispFun display)
{
    if (root == NULL || root->value == NULL)
    {
        return;
    }

    if (root->left != NULL)
    {
        gc_bst_display(root->left, display);
    }

    display(root->value);
    
    if (root->right != NULL)
    {
        gc_bst_display(root->right, display);
    }
    
}

/* 
     a
   b   e
  c d f g

*/

GCBNode *gc_bst_node(void *value)
{
    GCBNode *node = malloc(sizeof(GCBNode));
    if (node == NULL)
    {
        return NULL;
    }

    node->value = value; 
    node->left = NULL; 
    node->right = NULL;
    return node;
}