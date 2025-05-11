#include <stdlib.h>

#include "gc_logs.h"
#include "file.h"

FILE *TODOFILE;

FILE *todo_file()
{
    if (TODOFILE)
    {
        return TODOFILE;
    }

    char *filepath = ".todo";

    FILE *file = fopen(filepath, "ab");
    if (file == NULL)
    {
        GC_LOG("failed to open file\n");
        exit(EXIT_FAILURE);
    }
    TODOFILE = file;

    return file;
}

void todo_file_close()
{
    fclose(TODOFILE);
    TODOFILE = NULL;
}

int todo_file_isopen()
{
    return (TODOFILE != NULL);
}