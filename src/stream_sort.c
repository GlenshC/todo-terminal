#include "stream_sort.h"
#include "todo_sort/todo_sort.h"
#include <stdlib.h>
#include <string.h>
#include "todo_get_alg.h"
#include "gc_bsearch.h"

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
    if (list->sortingFunc == TODO_SORT_BEST)
    {
        if (list->isAccending <= 0)
        {
            list->isAccending = 1;
        }
        else
        {
            list->isAccending = -1;
        }
        pScore *scoreTable = todo_get_todouserenergy(&list->energy);
        time_t timeToday = list->timeToday;
        size_t size = list->size;
        for (size_t i =0; i < size; i++)
        {
            list->priorityScore[i] = todo_get_priorityScore(list, i, scoreTable, timeToday);
        }
    }
    gc_introsort(list, list->sortedList, list->size, list->sortingFunc);
}

void todo_sort_stream_remove(TodoList *list, GC_SORT_TYPE index, size_t size)
{
    GC_SORT_TYPE *arr = list->sortedList;
    if (size < 32) // linear search
    {
        for (size_t i = 0; i < size; i++)
        {
            if (arr[i] == index)
            {
                memmove(arr + i, arr + i + 1, (size - i - 1) * sizeof(*arr));
                arr[size-1] = index;
                return;
            }
        }
        return;
    }
    GC_SORT_TYPE i = gc_bsearch(arr, index, size);
    memmove(arr + i, arr + i + 1, (size - i - 1) * sizeof(*arr));
    arr[size - 1] = index;
}
