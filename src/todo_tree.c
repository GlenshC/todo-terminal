
#include <stdlib.h>
#include "todo_tree.h"
#include "gc_logs.h"

static TNode *todo_tree_node(void);
static void todo_tree_free_internals(TRoot *root);
static void todo_tree_display_internals(TodoList *list, TRoot *root, todotreeDispFun display, unsigned int *index);
static void todo_tree_pushNode(TodoList *list, TRoot *root, TNode *node, todotreeCmpFun compare);
static unsigned int todo_tree_at_internals(TodoList *list, TRoot *root, unsigned int index, unsigned int *countIndex);

TRoot *todo_tree_init()
{
    return (TNode *) todo_tree_node();
}

void todo_tree_free(TRoot **root)
{
    if (root == NULL)
    {
        return;
    }
    todo_tree_free_internals(*root);
    free(*root);
    *root = NULL;
}

void todo_tree_push(TodoList *list, TRoot *root, unsigned int value, todotreeCmpFun compare)
{
    if (root == NULL)
    {
        return;
    }
    
    // + when a is bigger (return > 0) store to right
    // - when b is bigger (return < 0) store to left
    // 0 when equal (return == 0) store to left
    TNode *node;
    for (node = root; node != NULL; )
    {
        if (node->valid == 0)
        {
            node->value = value;
            node->valid = 1;
            break;
        }

        int result = compare(list, value, node->value);
        if (result < 0)
        {
            node->balance--;
            
            if (node->left == NULL)
            {
                node->left = todo_tree_node();
            }
            node = node->left;
        }
        else
        {
            node->balance++;
            
            if (node->right == NULL)
            {
                node->right = todo_tree_node();
            }
            node = node->right;
        }
    }
    
}

void todo_tree_remove(TodoList *list, TRoot *root, unsigned int value, todotreeCmpFun compare)
{
    if (root == NULL)
    {
        return;
    }

    TNode *node = NULL, *shift = NULL, *insert = NULL, **dir = NULL;
    for (node = root; node != NULL; )
    {
        if (node->value == value)
        {
            
            if (node->left != NULL && node->balance < 0) // shift left
            {
                shift = node->left;
                node->value = shift->value;
                node->left = shift->left;

                if (shift->right)
                {
                    insert = node->right;
                }
                node->right = shift->right;
            }
            else if (node->right != NULL) // shift right
            {
                shift = node->right;
                node->value = shift->value;
                node->right = shift->right;
                
                if (shift->left)
                {
                    insert = node->left;
                }
                node->left = shift->left;
            }
            else
            {
                if (dir == NULL)
                {
                    node->value = 0;
                    node->valid = 0;
                    node->balance = 0;
                    node->left = NULL;
                    node->right = NULL;
                }
                else
                {
                    free(node);
                    *dir = NULL;
                }
            }

            if (insert)
            {
                todo_tree_pushNode(list, root, insert, compare);
            }
        }

        int result = compare(list, value, node->value);
        if (result < 0)
        {
            node->balance++;
            if (node->left == NULL)
            {
                break;
            }
            dir = &node->left;
            node = node->left;
        }
        else
        {
            node->balance--;
            if (node->right == NULL)
            {
                break;
            }
            dir = &node->right;
            node = node->right;
        }
        
    }
}

void todo_tree_display(TodoList *list, TRoot *root, todotreeDispFun display)
{
    unsigned int index = 0;
    todo_tree_display_internals(list, root, display, &index);
}

/* =================================
    Private Functions
   ================================= */
//

static void todo_tree_pushNode(TodoList *list, TRoot *root, TNode *node, todotreeCmpFun compare)
{
    if (root == NULL)
    {
        return;
    }
    
    // + when a is bigger (return > 0) store to right
    // - when b is bigger (return < 0) store to left
    // 0 when equal (return == 0) store to left
    TNode *currentNode;
    for (currentNode = root; currentNode != NULL; )
    {
        int result = compare(list, node->value, currentNode->value);
        if (result < 0)
        {
            currentNode->balance--;
            
            if (currentNode->left == NULL)
            {
                currentNode->left = node;
                break;
            }
            currentNode = currentNode->left;
        }
        else
        {
            currentNode->balance++;
            
            if (currentNode->right == NULL)
            {
                currentNode->right = node;
                break;
            }
            currentNode = currentNode->right;
        }

    }
}

static void todo_tree_display_internals(TodoList *list, TRoot *root, todotreeDispFun display, unsigned int *index)
{
    if (root == NULL || !(root->valid))
    {
        return;
    }

    if (root->left != NULL)
    {
        todo_tree_display_internals(list, root->left, display, index);
    }
    
    (*index)++;
    display(list, root->value);
    
    if (root->right != NULL)
    {
        todo_tree_display_internals(list, root->right, display, index);
    }
}

unsigned int todo_tree_at(TodoList *list, TRoot *root, unsigned int index)
{
    unsigned int countIndex = 0;
    if (index >= list->size)
    {
        GC_LOG("Beyond bounds.\n");
        return 0;
    }
    
    return todo_tree_at_internals(list, root, index, &countIndex);
}

static unsigned int todo_tree_at_internals(TodoList *list, TRoot *root, unsigned int index, unsigned int *countIndex)
{
    if (root == NULL || !(root->valid))
    {
        GC_LOG("Not found.\n");
        return 0;
    }

    if (root->left != NULL)
    {
        return todo_tree_at_internals(list, root->left, index, countIndex);
    }
    
    if (*countIndex == index)
    {
        printf("Found.\n");
        return root->value;
    }
    (*countIndex)++;
    
    if (root->right != NULL)
    {
        return todo_tree_at_internals(list, root->right, index, countIndex);
    }
    return 0;
}

//
static void todo_tree_free_internals(TRoot *root)
{
    if (root == NULL)
    {
        return;
    }

    if (root->left != NULL)
    {
        todo_tree_free_internals(root->left);
        free(root->left);
    }
    
    if (root->right != NULL)
    {
        todo_tree_free_internals(root->right);
        free(root->right);
    }
}


static TNode *todo_tree_node(void)
{
    TNode *node = malloc(sizeof(TNode));
    if (node == NULL)
    {
        return NULL;
    }

    node->value = 0;
    node->valid = 0;
    node->balance = 0;
    node->left = NULL; 
    node->right = NULL;
    return node;
}


/* compare functions */


