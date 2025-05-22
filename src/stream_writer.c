#include "stream_writer.h"
#include "todoio.h"
#include "bitwise.h"

/* 
    -- PADDING_START --
    
    size_t size;
    long long created[];
    long long deadline[];
    
    uint8_t titleSize[];
    char *title[];
    uint8_t descSize[];
    char *desc[];
    
    uint8_t priority[];
    uint8_t done[];

    -- PADDING_END--
*/
void stream_writer(TodoList *todolist, FILE *file)
{
    size_t bytesWritten;
    size_t n = todolist->size;

    // size & dates
    fwrite_align8(&n, sizeof(todolist->size), 1, file);
    fwrite_align8(todolist->created, sizeof(*todolist->created), n, file);
    fwrite_align8(todolist->deadline, sizeof(*todolist->deadline), n, file);

    // title
    bytesWritten = 0;
    fwrite_align8(todolist->titleSize, sizeof(*todolist->titleSize), n, file);
    for (size_t i = 0; i < n; i++)
    {
        bytesWritten += fwrite(todolist->title[i], sizeof(char), todolist->titleSize[i], file);
    }
    fwrite_pad8(bytesWritten, file);
    
    // description
    bytesWritten = 0;
    fwrite_align8(todolist->descSize, sizeof(*todolist->descSize), n, file);
    for (size_t i = 0, bytesWritten = 0; i < n; i++)
    {
        bytesWritten += fwrite(todolist->desc[i], sizeof(char), todolist->descSize[i], file);
    }
    fwrite_pad8(bytesWritten, file);

    // priority
    fwrite_align8(todolist->priority, sizeof(*todolist->priority), sizeofBytes(n, 2), file);
    fwrite_align8(todolist->done, sizeof(*todolist->done), sizeofBytes(n, 1), file);


    fwrite_align8(&TODO_FILE_PADDING_END, sizeof(size_t), 1, file);
}