#include "todoio.h"
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
#include <direct.h> 
#define mkdir(path, mode) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h> 
#endif

static const char ALIGNMENT_PADDING[512] = {};

void aligned_fwrite(const void * RESTRICT str, size_t size, size_t count, size_t alignment, FILE *file)
{
    fwrite(str, size, count, file);
    size_t alignment_padding = alignment - ((size*count) % alignment);
    if (alignment_padding % alignment)
    {
        fwrite(ALIGNMENT_PADDING, sizeof(char), alignment_padding, file);
    }

}

void aligned_fread(void * RESTRICT str, size_t size, size_t count, size_t alignment, FILE *file)
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



char *get_todo_file_path() 
{
    static char path[512] = "todo.todo";

#if defined(_WIN32) || defined(_WIN64)
    const char *home = getenv("USERPROFILE");
#else
    const char *home = getenv("HOME");
#endif

    if (!home) {
        fprintf(stderr, "Could not find home directory.\n");
        return path;
    }

#if defined(_WIN32) || defined(_WIN64)
    snprintf(path, sizeof(path), "%s\\AppData\\Roaming\\todo-terminal", home);
    _mkdir(path);
    snprintf(path, sizeof(path), "%s\\AppData\\Roaming\\todo-terminal\\todo.todo", home);
#else
    snprintf(path, sizeof(path), "%s/.todo-terminal", home);
    mkdir(path, 0700); 
    snprintf(path, sizeof(path), "%s/.todo-terminal/todo.todo", home);
#endif

    return path;
}
