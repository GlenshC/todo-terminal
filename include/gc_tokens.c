#include "gc_tokens.h"
#include <string.h>
int gc_tokenize(char *str, const char *delim, char *arr[], int n)
{
    char *saveptr, *token;
    int i;
    for (i = 0; i < n; str = NULL)
    {
        token = strtok_r(str, delim, &saveptr);
        if (token == NULL)
            break;

        arr[i++] = token;
    }

    return i;
}