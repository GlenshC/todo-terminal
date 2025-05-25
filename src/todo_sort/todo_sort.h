#ifndef TODO_SORTING_INTERFACE_H
#define TODO_SORTING_INTERFACE_H

#include "types.h"
#include <math.h>

// entry point
void gc_introsort(TodoList *list, GC_SORT_TYPE *indexArr, unsigned int size, SortingTypeEnum sort_type);

void gc_insertionsort_bytes(uint8_t *values, GC_SORT_TYPE *indexArr, unsigned int size, ascendingType isAscending, TodoListByteFields nbits);
void gc_qsort_bytes(uint8_t *values, GC_SORT_TYPE *indexArr, unsigned int size, ascendingType isAscending, TodoListByteFields nbits);

void gc_insertionsort_str(char **values, GC_SORT_TYPE *indexArr, unsigned int size, ascendingType isAscending);
void gc_qsort_str(char **values, GC_SORT_TYPE *indexArr, unsigned int size, ascendingType isAscending);

void gc_insertionsort_uint32(uint32_t *values, GC_SORT_TYPE *indexArr, unsigned int size, ascendingType isAscending);
void gc_qsort_uint32(uint32_t *values, GC_SORT_TYPE *indexArr, unsigned int size, ascendingType isAscending);

void swap(GC_SORT_TYPE *a, GC_SORT_TYPE *b);

#endif