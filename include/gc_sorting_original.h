
#ifndef GC_SORTING_INTERFACE_H
#define GC_SORTING_INTERFACE_H

#define GC_SORT_TYPE int

void gc_introsort(GC_SORT_TYPE *arr, unsigned int size);
void gc_insertionsort(GC_SORT_TYPE *arr, unsigned int size);
void gc_heapsort(GC_SORT_TYPE *arr, unsigned int size);
void gc_qsort(GC_SORT_TYPE *arr, unsigned int size);

#endif // GC_SORTING_INTERFACE_H