#include "todo_sort.h"
#include "bitwise.h"

#define GC_STYPE uint8_t

static void gc_qsort_implementation(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high, int depth_limit, TodoListByteFields nbits); 
static int gc_qsort_partition(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high, TodoListByteFields nbits);
static int gc_get_pivotIndex(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high, TodoListByteFields nbits);
static void gc_sort_heapify(GC_STYPE *values, GC_SORT_TYPE *indexArr, unsigned int size, unsigned int index, TodoListByteFields nbits);
static void gc_heapsort(GC_STYPE *values, GC_SORT_TYPE *indexArr, unsigned int size, TodoListByteFields nbits);

/* ================================ 
    QSORT 
   ================================*/

void gc_qsort_bytes(uint8_t *values, GC_SORT_TYPE *indexArr, unsigned int size, TodoListByteFields nbits)
{
    gc_qsort_implementation(values, indexArr, 0, size - 1, 2 * (int)log2(size), nbits);
}

static void gc_qsort_implementation(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high, int depth_limit, TodoListByteFields nbits) 
{
    if (depth_limit == 0)
    {
        gc_heapsort(values, indexArr + low, high, nbits);
    }
    if (low < high) {
        int pi = gc_qsort_partition(values, indexArr, low, high, nbits);

        gc_qsort_implementation(values, indexArr, low, pi - 1, depth_limit - 1, nbits);
        gc_qsort_implementation(values, indexArr, pi + 1, high, depth_limit - 1, nbits);
    }
}
/*

*/
static int gc_qsort_partition(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high, TodoListByteFields nbits) 
{
    GC_STYPE pivot = todo_getBit(values, indexArr[gc_get_pivotIndex(values, indexArr, low, high, nbits)], nbits);
    
    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        
        if (todo_getBit(values, indexArr[j], nbits) < pivot) {
            i++;
            swap(&indexArr[i], &indexArr[j]);
        }
    }
    
    swap(&indexArr[i + 1], &indexArr[high]);  
    return i + 1;
}

static int gc_get_pivotIndex(GC_STYPE *values, GC_SORT_TYPE *indexArr, int low, int high, TodoListByteFields nbits)
{
    int mid = low + (high - low) / 2;
    GC_SORT_TYPE 
        a = indexArr[low],
        b = indexArr[mid],
        c = indexArr[high];

        
    if (todo_getBit(values, a, nbits) > todo_getBit(values, c, nbits))
    {
        swap(indexArr+low, indexArr+high);
    }
    if (todo_getBit(values, b, nbits) > todo_getBit(values, c, nbits))
    {
        swap(indexArr+mid, indexArr+high);
    }
    if (todo_getBit(values, a, nbits) > todo_getBit(values, b, nbits))
    {
        swap(indexArr+low, indexArr+mid);
    }
    swap(indexArr+mid, indexArr+high);

    return high;
}


/* ================================ 
    HEAPSORT 
   ================================*/

static void gc_heapsort(GC_STYPE *values, GC_SORT_TYPE *indexArr, unsigned int size, TodoListByteFields nbits)
{
    for (unsigned int i = size/2; i-- > 0;)
    {
        gc_sort_heapify(values, indexArr, size, i, nbits);
    }

    for (unsigned int i = size; i-- > 0;)
    {
        swap(indexArr, indexArr+i);

        gc_sort_heapify(values, indexArr, i, 0, nbits);
    }
}

static void gc_sort_heapify(GC_STYPE *values, GC_SORT_TYPE *indexArr, unsigned int size, unsigned int index, TodoListByteFields nbits)
{
    unsigned int largest = index;
    unsigned int left = (2 * index) + 1;
    unsigned int right = (2 * index) + 2;

    if (left < size && todo_getBit(values, indexArr[left], nbits) > todo_getBit(values, indexArr[largest], nbits))
    {
        largest = left;
    }

    if (right < size && todo_getBit(values, indexArr[right], nbits) > todo_getBit(values, indexArr[largest], nbits))
    {
        largest = right;
    }

    if (largest != index)
    {
        swap(indexArr+largest, indexArr+index);

        gc_sort_heapify(values, indexArr, size, largest, nbits);
    }
}


/* ================================ 
    INSERTION SORT 
   ================================*/
void gc_insertionsort_bytes(uint8_t *values, GC_SORT_TYPE *indexArr, unsigned int size, TodoListByteFields nbits)
{
    for (unsigned int i = 1; i < size; i++)
    {
        for (unsigned int j = i; j > 0; j--)
        {
            if (todo_getBit(values, indexArr[j], nbits) < todo_getBit(values, indexArr[j-1], nbits))
            {
                GC_SORT_TYPE temp = indexArr[j];
                indexArr[j] = indexArr[j-1];
                indexArr[j-1] = temp;
            }
        }
    }
}