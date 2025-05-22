#include "todo_cmp.h"
#include <string.h>
#include "bitwise.h"

int todo_tree_priorityScoreCompare(TodoList *list, unsigned int a, unsigned int b)
{
    int assending = list->isAccending;
    if (list->priorityScore[a] < list->priorityScore[b])
    {
        return 1*assending;
    }
    else if (list->priorityScore[a] > list->priorityScore[b])
    {
        return -1*assending;
    }    
    return todo_tree_priorityCompare(list, a, b);
}

int todo_tree_priorityCompare(TodoList *list, unsigned int a, unsigned int b)
{
    int assending = list->isAccending;
    uint8_t byteA = getByte_arr(list->priority, a, TODO_PRIORITY_BITS);
    uint8_t byteB = getByte_arr(list->priority, b, TODO_PRIORITY_BITS);
    a = readBits(byteA, bitIndex(a, TODO_PRIORITY_BITS), TODO_PRIORITY_BITS);
    b = readBits(byteB, bitIndex(b, TODO_PRIORITY_BITS), TODO_PRIORITY_BITS);

    if (a < b)
    {
        return -1*assending;
    }
    else if (a > b)
    {
        return 1*assending;
    }    
    return 0;
}

int todo_tree_defaultCompare(TodoList *list, unsigned int a, unsigned int b)
{
    int assending = list->isAccending;
    if (a < b)
    {
        return -1*assending;
    }
    else if (a > b)
    {
        return 1*assending;
    }    
    return 0;
}

int todo_tree_deadlineCompare(TodoList *list, unsigned int a, unsigned int b)
{
    int assending = list->isAccending;
    if (list->deadline[a] < list->deadline[b])
    {
        return -1*assending;
    }
    else if (list->deadline[a] > list->deadline[b])
    {
        return 1*assending;
    }    
    return todo_tree_priorityCompare(list, a, b);
}

int todo_tree_createdCompare(TodoList *list, unsigned int a, unsigned int b)
{
    int assending = list->isAccending;
    if (list->created[a] < list->created[b])
    {
        return -1*assending;
    }
    else if (list->created[a] > list->created[b])
    {
        return 1*assending;
    }    
    return todo_tree_priorityCompare(list, a, b);
}

int todo_tree_titleCompare(TodoList *list, unsigned int a, unsigned int b)
{
    int assending = list->isAccending;
    return strcmp(list->title[a], list->title[b])*assending;
}