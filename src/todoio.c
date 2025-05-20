#include "todoio.h"
#include <string.h>

static const char ALIGNMENT_PADDING[512] = {};

void aligned_fwrite(const void * restrict str, size_t size, size_t count, size_t alignment, FILE *file)
{
    fwrite(str, size, count, file);
    size_t alignment_padding = alignment - ((size*count) % alignment);
    if (alignment_padding % alignment)
    {
        fwrite(ALIGNMENT_PADDING, sizeof(char), alignment_padding, file);
    }

}

void aligned_fread(void * restrict str, size_t size, size_t count, size_t alignment, FILE *file)
{
    size_t alignment_padding = alignment - ((size*count) % alignment);    
    fread(str, size, count, file);
    if (alignment_padding % alignment)
    {
        fseek(file, (long)alignment_padding, SEEK_CUR); // unsafe
    }
}

void fwrite_padding(size_t bytesWritten, size_t alignment, FILE *file)
{
    size_t alignment_padding = alignment - (bytesWritten % alignment);
    if (alignment_padding % alignment)
    {
        fwrite(ALIGNMENT_PADDING, sizeof(char), alignment_padding, file);
    }
}

void fread_padding(size_t bytesRead, size_t alignment, FILE *file)
{
    size_t alignment_padding = alignment - (bytesRead % alignment);
    if (alignment_padding % alignment)
    {
        fseek(file, (long)alignment_padding, SEEK_CUR); // unsafe
    }
}

char *todo_getinput(char *buffer, int maxCount)
{
    char *res = fgets(buffer, maxCount, stdin);
    if (res == NULL)
    {
        return NULL;
    }
    buffer[strcspn(buffer, "\n")] = 0;
    
    return res;
}