#include "stream.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gc_logs.h"

static const char *TODO_FILE_HEADER = "TODO";
static const size_t TODO_FILE_PADDING = 0xCCCCCCCCCCCCCCCC;

static int todo_stream_write_new(TodoT *todo);
static void todo_stream_write_header(FILE *file);
static int todo_stream_check_header(FILE *file);

// FILE *ReadFile;
// FILE *WriteFile;

/* FILE *todo_stream_init()
{
    
}

void todo_stream_close()
{

}

int todo_stream_isopen()
{

} */

// WRITING PROCEDURE
// create temp file tmpfile()
// open todo file
// check if header exist
// get element
// write to temp the new element
// do to other stuff
int todo_stream_write(TodoT *todo)
{
    FILE *readwrite;
    FILE *tempFile;
    readwrite = fopen("todo.todo", "rb");
    if (readwrite == NULL)
    {
        GC_LOG("Todo not found. Creating new todo.");
        return todo_stream_write_new(todo);
    }
    
    tempFile = fopen("temp.temptodo", "wb");
    if (tempFile == NULL)
    {
        GC_LOG("temp failed");
        free(readwrite);
        return 2;
    }
    
        
    size_t n;
    size_t newN;
    char **strings;
    size_t *stringSizes;

    if (todo_stream_check_header(readwrite))
    {
        GC_LOG("Wrong header, creating new file.");
        return todo_stream_write_new(todo);
    }

    todo_stream_write_header(tempFile);
    
    // get number of elements
    fread(&n, sizeof(size_t), 1, readwrite);
    newN = n+1;
    fwrite(&newN, sizeof(size_t), 1, tempFile);

    // get title sizes
    stringSizes = malloc(sizeof(size_t) * newN);
    strings = malloc(sizeof(char *) * newN);
    fread(stringSizes, sizeof(size_t), n, readwrite);
    fwrite(stringSizes, sizeof(size_t), n, tempFile);
    fwrite(&todo->titleSize, sizeof(size_t), 1, tempFile);
    
    // get titles
    for (size_t i = 0; i < n; i++)
    {
        size_t size = stringSizes[i];
        strings[i] =  malloc(sizeof(char) * size);
        
        fread(strings[i], sizeof(char), size, readwrite);
        fwrite(strings[i], sizeof(char), size, tempFile);
    }
    fwrite(todo->title, sizeof(char), todo->titleSize, tempFile);
    
    for (size_t i = 0; i < n; i++)
    {
        free(strings[i]);
    }
    
    // get desc sizes
    fread(stringSizes, sizeof(size_t), n, readwrite);
    fwrite(stringSizes, sizeof(size_t), n, tempFile);
    fwrite(&todo->descSize, sizeof(size_t), 1, tempFile);
    
    // get descs
    for (size_t i = 0; i < n; i++)
    {
        size_t size = stringSizes[i];
        strings[i] =  malloc(sizeof(char) * size);
        
        fread(strings[i], sizeof(char), size, readwrite);
        fwrite(strings[i], sizeof(char), size, tempFile);
    }
    fwrite(todo->desc, sizeof(char), todo->descSize, tempFile);

    for (size_t i = 0; i < n; i++)
    {
        free(strings[i]);        
    }
    free(strings);

    fread(stringSizes, sizeof(size_t), n, readwrite);
    fwrite(stringSizes, sizeof(size_t), n, tempFile);
    fwrite(&todo->priority, sizeof(size_t), 1, tempFile);
    
    fread(stringSizes, sizeof(long long), n, readwrite);
    fwrite(stringSizes, sizeof(long long), n, tempFile);
    fwrite(&todo->created, sizeof(long long), 1, tempFile);
    
    fread(stringSizes, sizeof(long long), n, readwrite);
    fwrite(stringSizes, sizeof(long long), n, tempFile);
    fwrite(&todo->deadline, sizeof(long long), 1, tempFile);

    free(stringSizes);

    fwrite(&TODO_FILE_PADDING, sizeof(size_t), 1, tempFile);
    fclose(tempFile);
    fclose(readwrite);


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
int todo_stream_read(TodoList *todolist)
{
    if (todolist == NULL)
    {
        return -1;
    }
    FILE *readwrite;
    size_t size;
    size_t *titleSize;
    char **title;
    size_t *descSize;
    char **desc;
    size_t *priority;
    long long *created;
    long long *deadline;
    todolist->size = 0;
    
    
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
    fread(&size, sizeof(size_t), 1, readwrite);
    
    // malloc
    titleSize   = malloc(sizeof(size_t) * size);
    title       = malloc(sizeof(char *) * size);
    descSize    = malloc(sizeof(size_t) * size);
    desc        = malloc(sizeof(char *) * size);
    priority    = malloc(sizeof(size_t) * size);
    created     = malloc(sizeof(long long) * size);
    deadline    = malloc(sizeof(long long) * size);
    
    
    // get title sizes
    fread(titleSize, sizeof(size_t), size, readwrite);
    
    // get titles
    for (size_t i = 0; i < size; i++)
    {
        size_t n = titleSize[i];
        title[i] =  malloc(sizeof(char) * (n+1));
        
        fread(title[i], sizeof(char), n, readwrite);
        title[i][n] = 0;
    }
    
    // get desc sizes
    fread(descSize, sizeof(size_t), size, readwrite);
    
    // get descs
    for (size_t i = 0; i < size; i++)
    {
        size_t n = descSize[i];
        desc[i] =  malloc(sizeof(char) * (n+1));
        
        fread(desc[i], sizeof(char), n, readwrite);
        desc[i][n] = 0;
    }

    fread(priority, sizeof(size_t), size, readwrite);
    fread(created, sizeof(long long), size, readwrite);    
    fread(deadline, sizeof(long long), size, readwrite);

    fclose(readwrite);

    todolist->size = size;
    todolist->titleSize = titleSize;
    todolist->title = title;
    todolist->descSize = descSize;
    todolist->desc = desc;
    todolist->priority = priority;
    todolist->created = created;
    todolist->deadline = deadline;

    return 0;
}

void todo_stream_free_todolist(TodoList *todolist)
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
    
    todolist->size = 0;
    todolist->titleSize = NULL;
    todolist->title = NULL;
    todolist->descSize = NULL;
    todolist->desc = NULL;
    todolist->priority = NULL;
    todolist->created = NULL;
    todolist->deadline = NULL;
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


static int todo_stream_write_new(TodoT *todo)
{
    FILE *readwrite = fopen("todo.todo", "wb");
    if (readwrite == NULL)
    {
        return 1;
    }

    size_t n = 1;

    todo_stream_write_header(readwrite);
    
    fwrite(&n, sizeof(size_t), 1, readwrite);
    
    fwrite(&todo->titleSize, sizeof(size_t), 1, readwrite);
    fwrite(todo->title, sizeof(char), todo->titleSize, readwrite);
    
    fwrite(&todo->descSize, sizeof(size_t), 1, readwrite);
    fwrite(todo->desc, sizeof(char), todo->descSize, readwrite);
    
    fwrite(&todo->priority, sizeof(size_t),     1,  readwrite);
    fwrite(&todo->created,  sizeof(long long),  1,  readwrite);
    fwrite(&todo->deadline, sizeof(long long),  1,  readwrite);
    
    fwrite(&TODO_FILE_PADDING, sizeof(size_t), 1, readwrite);
    
    fclose(readwrite);

    return 0;
}

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
