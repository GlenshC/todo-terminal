#include "gc_bsearch.h"
static unsigned int gc_bsearch_implementation(GC_SORT_TYPE *arr, GC_SORT_TYPE value, unsigned int low, unsigned int high);

unsigned int gc_bsearch(GC_SORT_TYPE *arr, GC_SORT_TYPE value, unsigned int size)
{
    return gc_bsearch_implementation(arr, value, 0, size-1);
}

static unsigned int gc_bsearch_implementation(GC_SORT_TYPE *arr, GC_SORT_TYPE value, unsigned int low, unsigned int high)
{

    while (low <= high)
    {
        unsigned int mid = low + (high - low) / 2;

        if (arr[mid] == value)
            return mid;
        else if (value < arr[mid])
        {
            high = mid - 1;
        }
        else
        {
            low = mid + 1;
        }
    }
    return 0;
}