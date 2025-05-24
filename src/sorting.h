#ifndef TODO_STREAM_SORTING_H
#define TODO_STREAM_SORTING_H

#include "types.h"

GC_SORT_TYPE *todo_sort_stream_init(size_t size);
void todo_sort_stream(TodoList *list);
// void todo_sort_stream_remove(TodoList *arr, unsigned int index, size_t size);

#endif