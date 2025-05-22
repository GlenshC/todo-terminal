#include "stream_reader.h"
#include "todoio.h"
#include "todo_cmp.h"
#include "bitwise.h"

#include <stdlib.h>
#include <time.h>

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
//
void stream_reader(TodoList *todolist, FILE *file)
{
        // get number of elements
    size_t size, bytesRead;
    fread_align8(&size, sizeof(todolist->size), 1, file);
    
    // malloc
    todo_stream_init(todolist, size);
    todolist->size = size;
    
    uint32_t *created  = todolist->created;
    uint32_t *deadline = todolist->deadline;

    uint8_t *titleSize   = todolist->titleSize;
    char **title        = todolist->title;
    uint8_t *descSize    = todolist->descSize;
    char **desc         = todolist->desc;
    
    uint8_t *priority   = todolist->priority;
    uint8_t *done   = todolist->done;
    
    // get title sizes
    fread_align8(created, sizeof(*todolist->created), size, file);    
    fread_align8(deadline, sizeof(*todolist->deadline), size, file);

    fread_align8(titleSize, sizeof(*todolist->titleSize), size, file);
    
    // get titles
    bytesRead = 0;
    for (size_t i = 0; i < size; i++)
    {
        size_t n = titleSize[i];
        title[i] =  malloc(sizeof(char) * (n+1));
        
        bytesRead += fread(title[i], sizeof(char), n, file);
        title[i][n] = 0;
    }
    fread_pad8(bytesRead, file);
    
    // get desc sizes
    fread_align8(descSize, sizeof(*todolist->descSize), size, file);
    
    // get descs
    bytesRead = 0;
    for (size_t i = 0; i < size; i++)
    {
        size_t n = descSize[i];
        desc[i] =  malloc(sizeof(char) * (n+1));
        
        bytesRead += fread(desc[i], sizeof(char), n, file);
        desc[i][n] = 0;
    }
    fread_pad8(bytesRead, file);

    fread_align8(priority, sizeof(*todolist->priority), sizeofBytes(size, 2), file);
    fread_align8(done, sizeof(*todolist->done), sizeofBytes(size, 1), file);
}


// initializes todolist stream
int todo_stream_init(TodoList *list, size_t capacity) // capacity update done
{
    if (list == NULL)
    {
        return -1;
    }

    if (capacity < 16)
    {
        capacity = STREAM_STARTING_CAPCITY;
    }
    else 
    {
        gc_binceil64(capacity);
    }

    // size and capacity
    list->capacity = capacity;
    list->size = 0;
    
    // mallocs
    list->titleSize = malloc(sizeof(*list->titleSize)   * capacity);
    if (list->titleSize == NULL)
    {
        return 1;
    }

    list->title     = malloc(sizeof(*list->title)       * capacity);
    if (list->title == NULL)
    {
        return 1;
    }
    
    list->descSize  = malloc(sizeof(*list->descSize)    * capacity);
    if (list->descSize == NULL)
    {
        return 1;
    }
    
    list->desc      = malloc(sizeof(*list->desc)        * capacity);
    if (list->desc == NULL)
    {
        return 1;
    }

    list->priority  = malloc(sizeof(*list->priority)    * capacity);
    if (list->priority == NULL)
    {
        return 1;
    }

    list->created   = malloc(sizeof(*list->created)     * capacity);
    if (list->created == NULL)
    {
        return 1;
    }

    list->deadline  = malloc(sizeof(*list->deadline)    * capacity);
    if (list->deadline == NULL)
    {
        return 1;
    }
    
    list->priorityScore = malloc(sizeof(*list->priorityScore)*capacity);
    if (list->priorityScore == NULL)
    {
        return 1;
    }
    list->done = malloc(sizeof(*list->done)*capacity);
    if (list->done == NULL)
    {
        return 1;
    }
    
    list->sortedList = NULL;
    list->energy = 0;
    list->isAccending = 1;
    list->timeToday = time(NULL);
    list->sortingFunc = todo_tree_defaultCompare;

    return 0;
}