
#include "todo_sort.h"
#define GC_STYPE uint32_t

static void gc_qsort_implementation(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high, int depth_limit); 
static int gc_qsort_partition(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high);
static int gc_get_pivotIndex(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high);
static void gc_sort_heapify(GC_STYPE *values, GC_SORT_TYPE *indexArr, unsigned int size, unsigned int index);
static void gc_heapsort(GC_STYPE *values, GC_SORT_TYPE *indexArr, unsigned int size);

/* ================================ 
    QSORT 
   ================================*/

void gc_qsort_uint32(uint32_t *values, GC_SORT_TYPE *indexArr, unsigned int size)
{
    gc_qsort_implementation(values, indexArr, 0, size - 1, 2 * (int)log2(size));
}

static void gc_qsort_implementation(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high, int depth_limit) 
{
    if (depth_limit == 0)
    {
        gc_heapsort(values, indexArr + low, high);
    }
    if (low < high) {
        int pi = gc_qsort_partition(values, indexArr, low, high);

        gc_qsort_implementation(values, indexArr, low, pi - 1, depth_limit - 1);
        gc_qsort_implementation(values, indexArr, pi + 1, high, depth_limit - 1);
    }
}
/*

*/
static int gc_qsort_partition(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high) 
{
    GC_STYPE pivot = values[indexArr[gc_get_pivotIndex(values, indexArr, low, high)]];
    
    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        if (values[indexArr[j]] < pivot) {
            i++;
            swap(&indexArr[i], &indexArr[j]);
        }
    }
    
    swap(&indexArr[i + 1], &indexArr[high]);  
    return i + 1;
}

static int gc_get_pivotIndex(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high)
{
    int mid = low + (high - low) / 2;
    GC_SORT_TYPE 
        a = indexArr[low],
        b = indexArr[mid],
        c = indexArr[high];

    if (values[a] > values[c])
    {
        swap(indexArr+low, indexArr+high);
    }
    if (values[b] > values[c])
    {
        swap(indexArr+mid, indexArr+high);
    }
    if (values[a] > values[b])
    {
        swap(indexArr+low, indexArr+mid);
    }
    swap(indexArr+mid, indexArr+high);

    return high;
}


/* ================================ 
    HEAPSORT 
   ================================*/

static void gc_heapsort(GC_STYPE *values, GC_SORT_TYPE *indexArr, unsigned int size)
{
    for (unsigned int i = size/2; i-- > 0;)
    {
        gc_sort_heapify(values, indexArr, size, i);
    }

    for (unsigned int i = size; i-- > 0;)
    {
        swap(indexArr, indexArr+i);

        gc_sort_heapify(values, indexArr, i, 0);
    }
}

static void gc_sort_heapify(GC_STYPE *values, GC_SORT_TYPE *indexArr, unsigned int size, unsigned int index)
{
    unsigned int largest = index;
    unsigned int left = (2 * index) + 1;
    unsigned int right = (2 * index) + 2;

    if (left < size && values[indexArr[left]] > values[indexArr[largest]])
    {
        largest = left;
    }

    if (right < size && values[indexArr[right]] > values[indexArr[largest]])
    {
        largest = right;
    }

    if (largest != index)
    {
        swap(indexArr+largest, indexArr+index);

        gc_sort_heapify(values, indexArr, size, largest);
    }
}


/* ================================ 
    INSERTION SORT 
   ================================*/
void gc_insertionsort_uint32(uint32_t *values, GC_SORT_TYPE *indexArr, unsigned int size)
{
    for (unsigned int i = 1; i < size; i++)
    {
        for (unsigned int j = i; j > 0; j--)
        {
            if (values[indexArr[j]] < values[indexArr[j-1]])
            {
                GC_SORT_TYPE temp = indexArr[j];
                indexArr[j] = indexArr[j-1];
                indexArr[j-1] = temp;
            }
        }
    }
}