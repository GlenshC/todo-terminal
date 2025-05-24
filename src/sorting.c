#include "sorting.h"
#include "todo_sort/todo_sort.h"
#include <stdlib.h>

GC_SORT_TYPE *todo_sort_stream_init(size_t size)
{
    GC_SORT_TYPE *root = malloc(size * sizeof(*root));
    if (root == NULL)
    {
        return NULL;
    }
    for (size_t i = 0; i < size; i++)
    {
        root[i] = i;
    }
    return root;
}

void todo_sort_stream(TodoList *list)
{
    if (list == NULL || list->sortedList == NULL)
    {
        return;
    }
    gc_introsort(list, list->sortedList, list->size, list->sortingFunc);
}

// void todo_sort_stream_remove(TodoList *arr, unsigned int index, size_t size)
// {
//     return;
// }