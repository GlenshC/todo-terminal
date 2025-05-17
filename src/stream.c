#include "stream.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gc_logs.h"
#include "todo_tree.h"
#include "todo_cmp.h"


#define gc_binceil64(x) (--(x), (x) |= (x) >> 1, (x) |= (x) >> 2, (x) |= (x) >> 4, (x) |= (x) >> 8, (x) |= (x) >> 16, (x) |= (x) >> 32, ++(x))

#define STREAM_STARTING_CAPCITY 16
#define STREAM_CAPACITY_MARGIN 4

static const char *TODO_FILE_HEADER = "TODO";
static const size_t TODO_FILE_PADDING = 0xCCCCCCCCCCCCCCCC;


// static int todo_stream_write_new(TodoList *list); // depreciated
static void todo_stream_write_header(FILE *file);
static int todo_stream_check_header(FILE *file);
int todo_stream_init(TodoList *list, size_t capacity);

// static void todo_stream_display_sorted(TodoList *list, unsigned int value);


// WRITING PROCEDURE
// create temp file tmpfile()
// open todo file
// check if header exist
// get element
// write to temp the new element
// do to other stuff
int todo_stream_write(TodoList *list) // capacity update
{
    if (list == NULL)
    {
        return -1;
    }
    
    FILE *tempFile;
    size_t n = list->size;
    
    tempFile = fopen("temp.temptodo", "wb");
    if (tempFile == NULL)
    {
        GC_LOG("temp failed");
        return 2;
    }
    
    todo_stream_write_header(tempFile);
    
    // number of elements
    fwrite(&n, sizeof(list->size), 1, tempFile);

    // title sizes
    fwrite(list->titleSize, sizeof(*list->titleSize), n, tempFile);

    // titles
    for (size_t i = 0; i < n; i++)
    {
        fwrite(list->title[i], sizeof(char), list->titleSize[i], tempFile);
    }
    
    // description sizes
    fwrite(list->descSize, sizeof(*list->descSize), n, tempFile);

    // descriptions
    for (size_t i = 0; i < n; i++)
    {
        fwrite(list->desc[i], sizeof(char), list->descSize[i], tempFile);
    }

    // priority
    fwrite(list->priority, sizeof(*list->priority), n, tempFile);

    // created
    fwrite(list->created, sizeof(*list->created), n, tempFile);
    
    // deadline
    fwrite(list->deadline, sizeof(*list->deadline), n, tempFile);

    fwrite(&TODO_FILE_PADDING, sizeof(size_t), 1, tempFile);

    fclose(tempFile);
    remove("todo.todo");
    rename("temp.temptodo", "todo.todo");

    return 0;
        
}

// READING prodecure
// check if header exist;
// get padding
// get number of elements;
// get the other stuff
// get padding
int todo_stream_read(TodoList *todolist) // capacity update done
{
    if (todolist == NULL)
    {
        return -1;
    }
    FILE *readwrite;
    
    readwrite = fopen("todo.todo", "rb");
    if (readwrite == NULL)
    {
        GC_LOG("reading failed.\n");
        return 1;
    }
    
    if (todo_stream_check_header(readwrite))
    {
        GC_LOG("wrong header\n");
        fclose(readwrite);
        return 2;
    }
    
    // get number of elements
    size_t size;
    fread(&size, sizeof(todolist->size), 1, readwrite);
    
    // malloc
    todo_stream_init(todolist, size);
    todolist->size = size;
    size_t *titleSize   = todolist->titleSize;
    char **title        = todolist->title;
    size_t *descSize    = todolist->descSize;
    char **desc         = todolist->desc;
    uint8_t *priority   = todolist->priority;
    long long *created  = todolist->created;
    long long *deadline = todolist->deadline;
    
    // get title sizes
    fread(titleSize, sizeof(*todolist->titleSize), size, readwrite);
    
    // get titles
    for (size_t i = 0; i < size; i++)
    {
        size_t n = titleSize[i];
        title[i] =  malloc(sizeof(char) * (n+1));
        
        fread(title[i], sizeof(char), n, readwrite);
        title[i][n] = 0;
    }
    
    // get desc sizes
    fread(descSize, sizeof(*todolist->descSize), size, readwrite);
    
    // get descs
    for (size_t i = 0; i < size; i++)
    {
        size_t n = descSize[i];
        desc[i] =  malloc(sizeof(char) * (n+1));
        
        fread(desc[i], sizeof(char), n, readwrite);
        desc[i][n] = 0;
    }

    fread(priority, sizeof(*todolist->priority), size, readwrite);
    fread(created, sizeof(*todolist->created), size, readwrite);    
    fread(deadline, sizeof(*todolist->deadline), size, readwrite);

    fclose(readwrite);
    todo_stream_sort(todolist, todolist->sortingFunc);

    return 0;
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

    list->sortedList = NULL;
    list->isAccending = 1;
    list->sortingFunc = todo_tree_defaultCompare;

    return 0;
}

// grow todolist stream
int todo_stream_grow(TodoList *list, size_t newCapacity) // capacity update
{
    if (list == NULL)
    {
        return -1;
    }

    size_t capacity = newCapacity;
    gc_binceil64(capacity);
    
    size_t *titleSize   = realloc(list->titleSize, capacity * sizeof(*(list->titleSize)));
    if (titleSize == NULL)
    {
        return 1;
    }
    list->titleSize = titleSize;
    
    char **title        = realloc(list->title, capacity * sizeof(*(list->title)));
    if (title == NULL)
    {
        return 1;
    }
    list->title = title;
    
    size_t *descSize    = realloc(list->descSize, capacity * sizeof(*(list->descSize)));
    if (descSize == NULL)
    {
        return 1;
    }
    list->descSize = descSize;
    
    char **desc         = realloc(list->desc, capacity * sizeof(*(list->desc)));
    if (desc == NULL)
    {
        return 1;
    }
    list->desc = desc;
    
    uint8_t *priority    = realloc(list->priority, capacity * sizeof(*(list->priority)));
    if (priority == NULL)
    {
        return 1;
    }
    list->priority = priority;
    
    long long *created  = realloc(list->created, capacity * sizeof(*(list->created)));
    if (created == NULL)
    {
        return 1;
    }
    list->created = created;
    
    long long *deadline = realloc(list->deadline, capacity * sizeof(*(list->deadline)));
    if (deadline == NULL)
    {
        return 1;
    }
    list->deadline = deadline;
    
    list->capacity = capacity;

    return 0;
}


int todo_stream_push(TodoList *list, TodoT *todo)
{
    if (list == NULL || todo == NULL)
    {
        return -1;
    }

    if (list->size >= TODO_STREAM_MAX_TODOS)
    {
        GC_LOG("TODO PUSH FAILED: MAX CAPACITY!\n");
        return -2;
    }

    if ((list->size + STREAM_CAPACITY_MARGIN) >= list->capacity)
    {
        todo_stream_grow(list, list->size + STREAM_CAPACITY_MARGIN);
    }
    
    size_t index = (list->size); 
    
    list->titleSize[index]  = todo->titleSize;
    list->title[index]      = todo->title;
    list->descSize[index]   = todo->descSize;
    list->desc[index]       = todo->desc;
    list->priority[index]   = todo->priority;
    list->created[index]    = todo->created;
    list->deadline[index]   = todo->deadline;
    
    (list->size)++;
    return 0;
}

// fast remove
int todo_stream_remove(TodoList *list, size_t index)
{
    if (list == NULL)
    {
        return -1;
    }
    if (index >= list->size)
    {
        printf("Invalid Operation: Out-of-Range.\n");
        return -2;
    }
    //void todo_tree_remove(TodoList *list, TRoot *root, unsigned int value, todotreeCmpFun compare);
    (list->size)--;
    size_t end = list->size;

    
    todo_tree_remove(list, list->sortedList, index, todo_tree_priorityCompare);
    
    if (index != end)
    {
        todo_tree_remove(list, list->sortedList, end, todo_tree_priorityCompare);
        
        list->titleSize[index]  = list->titleSize[end];
        list->title[index]      = list->title[end];
        list->descSize[index]   = list->descSize[end];
        list->desc[index]       = list->desc[end];
        list->priority[index]   = list->priority[end];
        list->created[index]    = list->created[end];
        list->deadline[index]   = list->deadline[end];
        
        todo_tree_push(list, list->sortedList, index, todo_tree_priorityCompare);
    }
    
    
    
    // @bst remove the index and add index again
    
    return 0;
}

// removes by shifting
int todo_stream_remove_inorder(TodoList *list, size_t index)
{
    if (list == NULL)
    {
        return -1;
    }
    
    (list->size)--;
    size_t end = list->size;

    for (size_t i = index; i < end; i++)
    {
        list->titleSize[i]  = list->titleSize[i+1];
        list->title[i]      = list->title[i+1];
        list->descSize[i]   = list->descSize[i+1];
        list->desc[i]       = list->desc[i+1];
        list->priority[i]   = list->priority[i+1];
        list->created[i]    = list->created[i+1];
        list->deadline[i]   = list->deadline[i+1];
    }
    /* 
        queue the remove cmd
        an array of indices which we shift
    */
    
    return 0;
}


// frees todolist
void todo_stream_free_todolist(TodoList *todolist) // capacity update
{
    if (NULL == todolist && 0 == todolist->size)
    {
        GC_LOG("todo list empty");
        return;
    }
    size_t size = todolist->size;
    
    free(todolist->titleSize);
    for (size_t i = 0; i < size; i++)
    {
        free(todolist->title[i]);
    }
    free(todolist->title);
    
    free(todolist->descSize);
    for (size_t i = 0; i < size; i++)
    {
        free(todolist->desc[i]);
    }
    free(todolist->desc);
    
    free(todolist->priority);
    free(todolist->created);
    free(todolist->deadline);

    todo_tree_free(&todolist->sortedList);
    
    todolist->size = 0;
    todolist->capacity = 0;
    todolist->titleSize = NULL;
    todolist->title = NULL;
    todolist->descSize = NULL;
    todolist->desc = NULL;
    todolist->priority = NULL;
    todolist->created = NULL;
    todolist->deadline = NULL;
    todolist->sortedList = NULL;
}

void todo_stream_sort(TodoList *list, todotreeCmpFun compare)
{
    if (compare == NULL)
    {
        return;
    }
    if (list->sortedList != NULL)
    {
        todo_tree_free(&list->sortedList);
    }
    
    TRoot *root = todo_tree_init();
    list->sortedList = root;

    size_t size = list->size;
    for (size_t i =0; i < size; i++)
    {
        todo_tree_push(list, root, i, compare);
    }
}

/* ==============================
    Private Functions 
   ==============================*/

   
/* 

static int todo_stream_write_new(TodoList *list)
{
    if (list == NULL)
    {
        return -1;
    }

    FILE *readwrite = fopen("todo.todo", "wb");
    if (readwrite == NULL)
    {
        return 1;
    }

    size_t size = list->size;

    todo_stream_write_header(readwrite);
    
    fwrite(&size, sizeof(list->size), 1, readwrite);

    if (size > 0)
    {
        // title sizes
        fwrite(list->titleSize, sizeof(*list->titleSize), size, readwrite);
    
        // titles
        for (size_t i = 0; i < size; i++)
        {
            fwrite(list->title[i], sizeof(*list->title), list->titleSize[i], readwrite);
        }
        
        // description sizes
        fwrite(list->descSize, sizeof(*list->descSize), size, readwrite);
    
        // descriptions
        for (size_t i = 0; i < size; i++)
        {
            fwrite(list->desc[i], sizeof(*list->desc), list->descSize[i], readwrite);
        }
    
        // priority
        fwrite(list->priority, sizeof(*list->priority), size, readwrite);
    
        // created
        fwrite(list->created, sizeof(*list->created), size, readwrite);
        
        // deadline
        fwrite(list->deadline, sizeof(*list->deadline), size, readwrite);
    }

    fwrite(&TODO_FILE_PADDING, sizeof(size_t), 1, readwrite);
    
    fclose(readwrite);

    return 0;
} */

static void todo_stream_write_header(FILE *file)
{
    fwrite(TODO_FILE_HEADER, sizeof(char), 4, file);
    fwrite(&TODO_FILE_PADDING, sizeof(size_t), 1, file);
}

static int todo_stream_check_header(FILE *file)
{
    // check header;
    char header[5] = {};
    size_t padding;

    fread(header, sizeof(char), 4, file);
    if (strcmp(header, TODO_FILE_HEADER) != 0)
    {
        return 1;
    }
    
    // check padding;
    fread(&padding, sizeof(size_t), 1, file);
    if (padding != 0xCCCCCCCCCCCCCCCC)
    {
        return 2;
    }

    return 0;
}


/* 
    //.todo header must exist or else; 
    TODO // must be 4 bytes

    0xCCCCCCCCCCCCCCCC pading
    
    size_t n
    
    size_t titleSize[];
    char *title[];
    
    size_t descSize[];
    char *desc[];
        
    size_t priority[];
    time_t created[];
    time_t deadline[];
    
    0xCCCCCCCCCCCCCCCC pading
*/

/* 
{
typedef struct TodoT
{
    size_t titleSize;
    size_t descSize;
    size_t priority;
    char *title;
    char *desc;
    time_t created;
    time_t deadline;
} TodoT; // 56bytes - nopads

*/
