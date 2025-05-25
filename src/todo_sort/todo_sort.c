#include "todo_sort.h"
#include "gc_logs.h"
void swap(GC_SORT_TYPE *a, GC_SORT_TYPE *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

void gc_introsort(TodoList *list, GC_SORT_TYPE *indexArr, unsigned int size, SortingTypeEnum sort_type)
{
    if (size < 16)
    {
        switch(sort_type)
        {
            case TODO_SORT_DEADLINE:
            {
                gc_insertionsort_uint32(list->deadline, indexArr, size, list->isAccending);
                break;
            }
            case TODO_SORT_PRIORITY:
            {
                gc_insertionsort_bytes(list->priority, indexArr, size, list->isAccending, TODO_FIELD_PRIORITY);
                break;
            }
            case TODO_SORT_CREATED:
            {
                gc_insertionsort_uint32(list->created, indexArr, size, list->isAccending);
                break;
            }
            case TODO_SORT_TITLE:
            {
                gc_insertionsort_str(list->title, indexArr, size, list->isAccending);
                break;
            }
            case TODO_SORT_BEST:
            {
                gc_insertionsort_uint32(list->priorityScore, indexArr, size, list->isAccending);
                break;
            }
            default:
                break;
        }
        return;
    }

    switch(sort_type)
    {
        case TODO_SORT_DEADLINE:
        {
            gc_qsort_uint32(list->deadline, indexArr, size, list->isAccending);
            break;
        }
        case TODO_SORT_PRIORITY:
        {
            gc_qsort_bytes(list->priority, indexArr, size, list->isAccending, TODO_FIELD_PRIORITY);
            break;
        }
        case TODO_SORT_CREATED:
        {
            gc_qsort_uint32(list->created, indexArr, size, list->isAccending);
            break;
        }
        case TODO_SORT_TITLE:
        {
            gc_qsort_str(list->title, indexArr, size, list->isAccending);
            break;
        }
        case TODO_SORT_BEST:
        {
            gc_qsort_uint32(list->priorityScore, indexArr, size, list->isAccending);
            break;
        }
        default:
            break;
    }
}
