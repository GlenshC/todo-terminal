#include "todo_cmp.h"
#include <string.h>

int todo_tree_priorityCompare(TodoList *list, unsigned int a, unsigned int b)
{
    if (list->priority[a] < list->priority[b])
    {
        return 1;
    }
    else if (list->priority[a] > list->priority[b])
    {
        return -1;
    }    
    return 0;
}

int todo_tree_defaultCompare(TodoList *list, unsigned int a, unsigned int b)
{
    if (a < b)
    {
        return -1;
    }
    else if (a > b)
    {
        return 1;
    }    
    return 0;
}

int todo_tree_deadlineCompare(TodoList *list, unsigned int a, unsigned int b)
{
    if (list->deadline[a] < list->deadline[b])
    {
        return 1;
    }
    else if (list->deadline[a] > list->deadline[b])
    {
        return -1;
    }    
    return 0;
}

int todo_tree_createdCompare(TodoList *list, unsigned int a, unsigned int b)
{
    if (list->created[a] < list->created[b])
    {
        return 1;
    }
    else if (list->created[a] > list->created[b])
    {
        return -1;
    }    
    return 0;
}

int todo_tree_titleCompare(TodoList *list, unsigned int a, unsigned int b)
{
    return strcmp(list->title[a], list->title[b]);
}