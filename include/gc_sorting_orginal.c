
#include <math.h>
#include "gc_sorting.h"

static void swap(GC_SORT_TYPE* a, GC_SORT_TYPE* b);
static void gc_qsort_implementation(GC_SORT_TYPE *arr, int low, int high, int depth_limit); 
static int gc_qsort_partition(GC_SORT_TYPE *arr, int low, int high);
static int gc_get_pivotIndex(GC_SORT_TYPE *arr, int low, int high);
void gc_sort_heapify(GC_SORT_TYPE *arr, unsigned int size, unsigned int index);

/* ================================ 
    INTROSORT 
   ================================*/
void gc_introsort(GC_SORT_TYPE *arr, unsigned int size)
{
    if (size < 16)
    {
        gc_insertionsort(arr, size);
        return;
    }
    gc_qsort(arr, size);
}

/* ================================ 
    QSORT 
   ================================*/

void gc_qsort(GC_SORT_TYPE *arr, unsigned int size)
{
    gc_qsort_implementation(arr, 0, size - 1, 2 * (int)log2(size));
}

static void gc_qsort_implementation(GC_SORT_TYPE *arr, int low, int high, int depth_limit) 
{
    if (depth_limit == 0)
    {
        gc_heapsort(arr + low, high);
    }
    if (low < high) {
        int pi = gc_qsort_partition(arr, low, high);

        gc_qsort_implementation(arr, low, pi - 1, depth_limit - 1);
        gc_qsort_implementation(arr, pi + 1, high, depth_limit - 1);
    }
}
/*

*/
static int gc_qsort_partition(GC_SORT_TYPE *arr, int low, int high) 
{
    int pivot = arr[gc_get_pivotIndex(arr, low, high)];
    
    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    
    swap(&arr[i + 1], &arr[high]);  
    return i + 1;
}

static int gc_get_pivotIndex(GC_SORT_TYPE *arr, int low, int high)
{
    int mid = low + (high - low) / 2;
    if (arr[low] > arr[high])
    {
        swap(arr+low, arr+high);
    }
    if (arr[mid] > arr[high])
    {
        swap(arr+mid, arr+high);
    }
    if (arr[low] > arr[mid])
    {
        swap(arr+low, arr+mid);
    }
    swap(arr+mid, arr+high);
    return high;
}


/* ================================ 
    HEAPSORT 
   ================================*/

void gc_heapsort(GC_SORT_TYPE *arr, unsigned int size)
{
    for (unsigned int i = size/2; i-- > 0;)
    {
        gc_sort_heapify(arr, size, i);
    }

    for (unsigned int i = size; i-- > 0;)
    {
        GC_SORT_TYPE temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        gc_sort_heapify(arr, i, 0);
    }
}

void gc_sort_heapify(GC_SORT_TYPE *arr, unsigned int size, unsigned int index)
{
    unsigned int largest = index;
    unsigned int left = (2 * index) + 1;
    unsigned int right = (2 * index) + 2;

    if (left < size && arr[left] > arr[largest])
    {
        largest = left;
    }

    if (right < size && arr[right] > arr[largest])
    {
        largest = right;
    }

    if (largest != index)
    {
        GC_SORT_TYPE temp = arr[largest];
        arr[largest] = arr[index];
        arr[index] = temp;

        gc_sort_heapify(arr, size, largest);
    }
}


/* ================================ 
    INSERTION SORT 
   ================================*/
void gc_insertionsort(GC_SORT_TYPE *arr, unsigned int size)
{
    for (unsigned int i = 1; i < size; i++)
    {
        for (unsigned int j = i; j > 0; j--)
        {
            if (arr[j] < arr[j-1])
            {
                GC_SORT_TYPE temp = arr[j];
                arr[j] = arr[j-1];
                arr[j-1] = temp;
            }
        }
    }
}





/* =============================
    PRIVATE FUNCTIONS
   =============================*/
static void swap(GC_SORT_TYPE* a, GC_SORT_TYPE* b) {
    int t = *a;
    *a = *b;
    *b = t;
}