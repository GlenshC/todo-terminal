#include "stream.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gc_logs.h"
#include "todo_tree.h"
#include "todo_cmp.h"
#include "todo_args.h"
#include "todo_cmd.h"
#include "todo_version.h"
#include "terminal_colors.h"
#include "todo_error.h"
#include "todo_get_alg.h"

#include "todoio.h"

void todo_stream_recover(TodoList *list, FILE *file);
#define gc_binceil64(x) (--(x), (x) |= (x) >> 1, (x) |= (x) >> 2, (x) |= (x) >> 4, (x) |= (x) >> 8, (x) |= (x) >> 16, (x) |= (x) >> 32, ++(x))

#define STREAM_STARTING_CAPCITY 16
#define STREAM_CAPACITY_MARGIN 4

static const char *TODO_FILE_HEADER = "TODO";
static const char TODO_FILE_PADDING_START[] = {0xAB, 0xCD, 0xEF, 0xCC, 0xCC, 0xFE, 0xDC, 0xBA};
static const char TODO_FILE_PADDING_END[] =   {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC};


// static int todo_stream_write_new(TodoList *list); // depreciated
static void todo_stream_write_header(FILE *file);
static int todo_stream_check_header(FILE *file);
int todo_stream_init(TodoList *list, size_t capacity);
static void stream_reader(TodoList *todolist, FILE *file);
static void stream_writer(TodoList *todolist, FILE *file);
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

    tempFile = fopen("temp.temptodo", "wb");
    if (tempFile == NULL)
    {
        GC_LOG("temp failed");
        return 2;
    }

    todo_stream_write_header(tempFile);
    stream_writer(list, tempFile);

    fclose(tempFile);
    remove("todo.todo");
    rename("temp.temptodo", "todo.todo");

    return 0;
        
}


static void stream_writer(TodoList *todolist, FILE *file)
{
    size_t bytesWritten;
    size_t n = todolist->size;

    // number of elements
    fwrite_align8(&n, sizeof(todolist->size), 1, file);

    // title sizes
    fwrite_align8(todolist->titleSize, sizeof(*todolist->titleSize), n, file);

    // titles
    bytesWritten = 0;
    for (size_t i = 0; i < n; i++)
    {
        bytesWritten += fwrite(todolist->title[i], sizeof(char), todolist->titleSize[i], file);
    }
    fwrite_pad8(bytesWritten, file);
    
    // description sizes
    fwrite_align8(todolist->descSize, sizeof(*todolist->descSize), n, file);

    // descriptions
    bytesWritten = 0;
    for (size_t i = 0; i < n; i++)
    {
        bytesWritten += fwrite(todolist->desc[i], sizeof(char), todolist->descSize[i], file);
    }
    fwrite_pad8(bytesWritten, file);

    // priority
    fwrite_align8(todolist->priority, sizeof(*todolist->priority), n, file);

    // created
    fwrite_align8(todolist->created, sizeof(*todolist->created), n, file);
    
    // deadline
    fwrite_align8(todolist->deadline, sizeof(*todolist->deadline), n, file);

    fwrite_align8(&TODO_FILE_PADDING_END, sizeof(size_t), 1, file);
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
    
    int status = todo_stream_check_header(readwrite);
    // GC_LOG("status: %d\n", status);
    if (status < 0)
    {
        printf(
            TERMINAL_BOLD_RED 
            "Error: Save file version is pre-1.0b and cannot be loaded.\n" 
            TERMINAL_COLOR_RESET
        );
        fclose(readwrite);
        todo_cmd_clear(0, NULL);
    }
    else if (status) 
    {
        
        printf(
            TERMINAL_BOLD_RED "Wrong version.\n" TERMINAL_COLOR_RESET
            TERMINAL_COLOR_BLUE "Attempting to recover data.\n" TERMINAL_COLOR_RESET
        );
        todo_stream_recover(todolist, readwrite);
    
        fclose(readwrite);
        return 2;
    }

    stream_reader(todolist, readwrite);

    fclose(readwrite);
    // todo_stream_sort(todolist);

    return 0;
}

static void stream_reader(TodoList *todolist, FILE *file)
{
        // get number of elements
    size_t size, bytesRead;
    fread_align8(&size, sizeof(todolist->size), 1, file);
    
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

    fread_align8(priority, sizeof(*todolist->priority), size, file);
    fread_align8(created, sizeof(*todolist->created), size, file);    
    fread_align8(deadline, sizeof(*todolist->deadline), size, file);
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
    
    list->sortedList = NULL;
    list->energy = 0;
    list->isAccending = 1;
    list->timeToday = todo_get_timeToday();
    list->sortingFunc = todo_tree_defaultCompare;

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
    
    for (size_t i = 0; i < size; i++)
    {
        if (todolist->descSize[i])
        {
            free(todolist->desc[i]);
        }
    }
    free(todolist->descSize);

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
        todo_error(TODO_ERROR_INVALID_ID);
        return -1;
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




void todo_stream_sort(TodoList *list)
{
    todotreeCmpFun compare = list->sortingFunc;
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

    if (compare == todo_tree_priorityScoreCompare)
    {
        pScore *scoreTable = todo_get_todouserenergy(&list->energy);
        time_t timeToday = todo_get_timeToday();
        for (size_t i =0; i < size; i++)
        {
            list->priorityScore[i] = todo_get_priorityScore(list, i, scoreTable, timeToday);
            todo_tree_push(list, root, i, compare);
        }
    }
    else
    {
        for (size_t i =0; i < size; i++)
        {
            todo_tree_push(list, root, i, compare);
        }
    }    
    

}

void todo_stream_priorityScoreSort(TodoList *list, unsigned int energy)
{
    if (list->size < 1)
    {
        return;
    }
    list->sortingFunc = todo_tree_priorityScoreCompare;
    todotreeCmpFun compare = list->sortingFunc;

    if (list->sortedList != NULL)
    {
        todo_tree_free(&list->sortedList);
    }

    TRoot *root = todo_tree_init();
    list->sortedList = root;
    size_t size = list->size;

    list->energy = energy;

    pScore *scoreTable = todo_get_todouserenergy(&list->energy);
    time_t timeToday = todo_get_timeToday();
    for (size_t i =0; i < size; i++)
    {
        list->priorityScore[i] = todo_get_priorityScore(list, i, scoreTable, timeToday);
        
        todo_tree_push(list, root, i, compare);
    }

}

/* ==============================
    Private Functions 
   ==============================*/

void todo_stream_recover(TodoList *list, FILE *file)
{
    size_t padding;
    char firstChar;
    fseek(file, 0, SEEK_SET);

    int i = 0, maxi = 128; 
    for (;i < maxi; i++)
    {
        fread(&firstChar, sizeof(char), 1, file);
        if (firstChar == TODO_FILE_PADDING_START[0])
        {
            fseek(file, -1, SEEK_CUR);
            fread_align8(&padding, sizeof(size_t), 1, file);
            if (padding == *((size_t *)TODO_FILE_PADDING_START) || feof(file) || ferror(file))
            {
                break;
            }
            fseek(file, -7, SEEK_CUR);
        }
    }
    if (padding == *((size_t *)TODO_FILE_PADDING_START))
    {
        printf(TERMINAL_COLOR_GREEN "Data recovered.\n" TERMINAL_COLOR_RESET);
    }
    else 
    {
        fclose(file);
        printf(TERMINAL_COLOR_RED "Data recovery failed.\n" TERMINAL_COLOR_RESET);
        todo_cmd_clear(0, NULL);
        exit(EXIT_FAILURE);
    }
    
    stream_reader(list, file);
}

static void todo_stream_write_header(FILE *file)
{
    fwrite_align8(TODO_FILE_HEADER, sizeof(char), 4, file);
    fwrite_align8(TODO_APP_VERSION, sizeof(char), TODO_APP_VERSION_SIZE, file);
    fwrite_align8(TODO_FILE_PADDING_START, sizeof(size_t), 1, file);
}

static int todo_stream_check_header(FILE *file)
{
/* 
    error codes: 
    -1 wrong file
    -2 no pad detetected
     0 normal/successful
     1 invalid
    +2 represents version index in todo_version.h just subtract 2
*/
    char header[5] = {};
    char version[20] = {};
    size_t padding;
    int status = 0;

    fread_align8(header, sizeof(char), 4, file);
    if (strcmp(header, TODO_FILE_HEADER) != 0)
    {
        return -1;
    }

    fread_align8(version, sizeof(char), TODO_APP_VERSION_SIZE, file);
    version[TODO_APP_VERSION_SIZE] = 0;
    if (strcmp(version, TODO_APP_VERSION) != 0)
    {
        status = todo_version_id(version) + 1;
    }
    
    // check padding;
    fread_align8(&padding, sizeof(size_t), 1, file);
    if (padding != *((size_t *)TODO_FILE_PADDING_START))
    {
        return -2;
    }

    return status;
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

