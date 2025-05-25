
#include "todo_sort.h"
#define GC_STYPE uint32_t

static void gc_qsort_implementation(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high, int depth_limit, ascendingType isAscending); 
static int gc_qsort_partition(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high, ascendingType isAscending);
static int gc_get_pivotIndex(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high, ascendingType isAscending);
static void gc_sort_heapify(GC_STYPE *values, GC_SORT_TYPE *indexArr, unsigned int size, unsigned int index, ascendingType isAscending);
static void gc_heapsort(GC_STYPE *values, GC_SORT_TYPE *indexArr, unsigned int size, ascendingType isAscending);
static int compareFunc_uint32(GC_STYPE a, GC_STYPE b, ascendingType isAscending);
/* ================================ 
    QSORT 
   ================================*/

void gc_qsort_uint32(uint32_t *values, GC_SORT_TYPE *indexArr, unsigned int size, ascendingType isAscending)
{
    gc_qsort_implementation(values, indexArr, 0, size - 1, 2 * (int)log2(size), isAscending);
}

static void gc_qsort_implementation(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high, int depth_limit, ascendingType isAscending) 
{
    if (depth_limit == 0)
    {
        gc_heapsort(values, indexArr + low, high, isAscending);
    }
    if (low < high) {
        int pi = gc_qsort_partition(values, indexArr, low, high, isAscending);

        gc_qsort_implementation(values, indexArr, low, pi - 1, depth_limit - 1, isAscending);
        gc_qsort_implementation(values, indexArr, pi + 1, high, depth_limit - 1, isAscending);
    }
}
/*

*/
static int gc_qsort_partition(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high, ascendingType isAscending) 
{
    GC_STYPE pivot = values[indexArr[gc_get_pivotIndex(values, indexArr, low, high, isAscending)]];
    
    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        
        if (compareFunc_uint32(values[indexArr[j]], pivot, isAscending) < 0) {
            i++;
            swap(&indexArr[i], &indexArr[j]);
        }
    }
    
    swap(&indexArr[i + 1], &indexArr[high]);  
    return i + 1;
}

static int gc_get_pivotIndex(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high, ascendingType isAscending)
{
    int mid = low + (high - low) / 2;
    GC_STYPE 
        a = values[indexArr[low]],
        b = values[indexArr[mid]],
        c = values[indexArr[high]];

    
    if (compareFunc_uint32(a, c, isAscending) > 0)
    {
        swap(indexArr+low, indexArr+high);
    }
    if (compareFunc_uint32(b, c, isAscending) > 0)
    {
        swap(indexArr+mid, indexArr+high);
    }
    if (compareFunc_uint32(a, b, isAscending) > 0)
    {
        swap(indexArr+low, indexArr+mid);
    }
    swap(indexArr+mid, indexArr+high);

    return high;
}


/* ================================ 
    HEAPSORT 
   ================================*/

static void gc_heapsort(GC_STYPE *values, GC_SORT_TYPE *indexArr, unsigned int size, ascendingType isAscending)
{
    for (unsigned int i = size/2; i-- > 0;)
    {
        gc_sort_heapify(values, indexArr, size, i, isAscending);
    }

    for (unsigned int i = size; i-- > 0;)
    {
        swap(indexArr, indexArr+i);

        gc_sort_heapify(values, indexArr, i, 0, isAscending);
    }
}

static void gc_sort_heapify(GC_STYPE *values, GC_SORT_TYPE *indexArr, unsigned int size, unsigned int index, ascendingType isAscending)
{
    unsigned int largest = index;
    unsigned int left = (2 * index) + 1;
    unsigned int right = (2 * index) + 2;

    
    if (left < size && (compareFunc_uint32(values[indexArr[left]], values[indexArr[largest]],isAscending) > 0))
    {
        largest = left;
    }

    if (right < size && (compareFunc_uint32(values[indexArr[right]], values[indexArr[largest]],isAscending) > 0))
    {
        largest = right;
    }

    if (largest != index)
    {
        swap(indexArr+largest, indexArr+index);

        gc_sort_heapify(values, indexArr, size, largest, isAscending);
    }
}


/* ================================ 
    INSERTION SORT 
   ================================*/
void gc_insertionsort_uint32(uint32_t *values, GC_SORT_TYPE *indexArr, unsigned int size, ascendingType isAscending)
{
    for (unsigned int i = 1; i < size; i++)
    {
        for (unsigned int j = i; j > 0; j--)
        {
            
            if (compareFunc_uint32(values[indexArr[j]], values[indexArr[j-1]], isAscending) < 0)
            {
                GC_SORT_TYPE temp = indexArr[j];
                indexArr[j] = indexArr[j-1];
                indexArr[j-1] = temp;
            }
        }
    }
}


int compareFunc_uint32(GC_STYPE a, GC_STYPE b, ascendingType isAscending)
{
    if (a > b)
    {
        return 1 * isAscending;
    }
    else if (a < b)
    {
        return -1 * isAscending;
    }
    else
    {
        return 0;
    }
}