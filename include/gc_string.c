#include <stdio.h>
#include <stdlib.h>

#include "gc_string.h"

GC_String *GC_STR_ReadFile(const char* path)
{
    GC_String *buffer;    
    FILE *file;
    if (!(file = fopen(path, "r")))
    {
        printf("Failed to open file %s\n", path);
        return NULL;
    }
    long len;
    
    fseek(file, 0, SEEK_END);
    len = ftell(file);
    if (len < 0)
    {
        fclose(file);
        return NULL;
    }
    fseek(file, 0, SEEK_SET);

    
    buffer = (GC_String *)malloc(sizeof(GC_String) + (size_t)len);
    size_t bytesRead = fread(buffer->string, sizeof(char), (size_t)len, file);

    buffer->string[bytesRead] = '\0';
    buffer->len = bytesRead;

    fclose(file);

    printf("File %s read successfully.\n",path);

    return buffer;
}

size_t GC_STR_strcpy_internal(char *dest, const char *src, unsigned int n, unsigned int max, const char* file, int line) 
{
    size_t count = 0;
    do {
        if (n >= max) {
            *(dest+(n-1)) = 0;
            printf("%s:%d: BUFFER_OVERFLOW: Surpassed limit %d bytes\n", file, line, max);
            return count;
            break;
        }
        
        *(dest+(n++)) = *(src++);
        count++;
    } while (*src);
    
    *(dest+(n)) = 0;
    return count;
}

size_t GC_STR_nstrcpy_internal(char *dest, const char *src, unsigned int i, unsigned int n, unsigned int max, const char* file, int line) 
{
    size_t count = 0;
    do {
        if (i >= max) {
            *(dest+(i-1)) = 0;
            printf("%s:%d: BUFFER_OVERFLOW: Surpassed limit %d bytes\n", file, line, max);
            return count;
            break;
        }
        
        if (i >= n)
        break;
        
        *(dest+(i++)) = *(src++);
        count++;
    } while (*src);
    
    *(dest+(i)) = 0;
    return count;
}

// #define GC_STR_GetLastChar(str, c, n, max);
size_t GC_STR_GetLastChar_internals(const char *str, const char c, unsigned int n, unsigned int max, const char* file, int line)
{
    size_t index = 0;
    do
    {
        if (n >= max)
        {
            printf("%s:%d: BUFFER_OVERFLOW: Surpassed limit %d bytes\n", file, line, max);
            break;
        }

        if (*(str+n) == c)
            index = n;
        n++;
    } while (*(str+n));

    return index;
}


/* Implement read batch of file (malloc a batch of file)
G_String *G_ReadMultipleFiles(const char** paths, int n);
*/

// dev notes
// develop an safe input system for strings;
