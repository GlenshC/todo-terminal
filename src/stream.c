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
#include "stream_reader.h"
#include "stream_writer.h"
#include "bitwise.h"

#include "todoio.h"

// #define CLAMP(val, min, max) (((val) <= (min)) ? (min) : (((val) >= (max)) ? (max) : (val)))
#define UCLAMP(val, max) (((val) > (max)) ? (max) : (val))

void todo_stream_recover(TodoList *list, FILE *file);

static const char *TODO_FILE_HEADER = "TODO";
static const char TODO_FILE_PADDING_START[] = {0xAB, 0xCD, 0xEF, 0xCC, 0xCC, 0xFE, 0xDC, 0xBA};


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
#if defined(_WIN32) || defined(_WIN64)
    #define TEMP_FILE_NAME "temp.temptodo"
#else
    #define TEMP_FILE_NAME ".temp.temptodo"
#endif

uint8_t todo_getBit(uint8_t *arr, size_t index, TodoListByteFields fieldEnum)
{
    uint8_t byte = getByte_arr(arr, index, fieldEnum);
    return readBits(byte, bitIndex(index, fieldEnum), fieldEnum);
}

void todo_writeBit(uint8_t *arr, uint8_t value, size_t index, TodoListByteFields fieldEnum)
{
    uint8_t *byte = &getByte_arr(arr, index, fieldEnum);
    writeBit_ptr(byte, value, bitIndex(index, fieldEnum), fieldEnum);
}


/* ======================================
    Read
   ====================================== */
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
    
    readwrite = fopen(get_todo_file_path(), "rb");
    if (readwrite == NULL)
    {
        todo_stream_init(todolist, 0);
        GC_LOG("aareading failed.\n");
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

/* ======================================
    Write
   ====================================== */
int todo_stream_write(TodoList *list) // capacity update
{
    if (list == NULL)
    {
        return -1;
    }
    
    FILE *tempFile;

    tempFile = fopen(TEMP_FILE_NAME, "wb");
    if (tempFile == NULL)
    {
        GC_LOG("temp failed");
        return 2;
    }

    todo_stream_write_header(tempFile);
    stream_writer(list, tempFile);

    fclose(tempFile);
    remove(get_todo_file_path());
    rename(TEMP_FILE_NAME, get_todo_file_path());

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
    free(todolist->done);
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
    todolist->done = NULL;
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

    size_t index = (list->size);

    list->created[index]    = (todo->created + (SECONDS_IN_MIN - 1)) / SECONDS_IN_MIN;
    list->deadline[index]   = (todo->deadline + (SECONDS_IN_DAY - 1)) / SECONDS_IN_DAY;
    list->titleSize[index]  = UCLAMP(todo->titleSize, 255); 
    list->title[index]      = todo->title;
    list->descSize[index]   = UCLAMP(todo->descSize, 255);
    list->desc[index]       = todo->desc;

    todo_writeBit(list->priority, todo->priority, index, TODO_PRIORITY_BITS);
    todo_writeBit(list->done, todo->done, index, TODO_DONE_BITS);
    
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
    uint8_t *byteptr;
    uint8_t bit_end;

    todo_tree_remove(list, list->sortedList, index, todo_tree_priorityCompare);
    
    if (index != end)
    {
        todo_tree_remove(list, list->sortedList, end, todo_tree_priorityCompare);
        
        list->titleSize[index]  = list->titleSize[end];
        list->title[index]      = list->title[end];
        list->descSize[index]   = list->descSize[end];
        list->desc[index]       = list->desc[end];

        byteptr = &getByte_arr(list->priority, index, TODO_PRIORITY_BITS);
        bit_end = readBit_arr(list->priority, end, TODO_PRIORITY_BITS);
        writeBit_ptr(byteptr, bit_end, index, TODO_PRIORITY_BITS);
        
        byteptr = &getByte_arr(list->done, index, TODO_DONE_BITS);
        bit_end = readBit_arr(list->done, end, TODO_DONE_BITS);
        writeBit_ptr(byteptr, bit_end, index, TODO_DONE_BITS);

        list->created[index]    = list->created[end];
        list->deadline[index]   = list->deadline[end];
        
        todo_tree_push(list, list->sortedList, index, todo_tree_priorityCompare);
    }
    
    
    
    // @bst remove the index and add index again
    
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

/* 
    error codes: 
    -1 wrong file
    -2 no pad detetected
     0 normal/successful
     1 invalid
    +2 represents version index in todo_version.h just subtract 2
*/
static int todo_stream_check_header(FILE *file)
{
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


// grow todolist stream
/* // depreciated
int todo_stream_grow(TodoList *list, size_t newCapacity) // capacity update
{
    if (list == NULL)
    {
        return -1;
    }

    size_t capacity = newCapacity;
    gc_binceil64(capacity);
    
    uint8_t *titleSize   = realloc(list->titleSize, capacity * sizeof(*(list->titleSize)));
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
    
    uint8_t *descSize    = realloc(list->descSize, capacity * sizeof(*(list->descSize)));
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
 */

 /* 
 // depreciated
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
        list->done[i]   = list->done[i+1];
        list->created[i]    = list->created[i+1];
        list->deadline[i]   = list->deadline[i+1];
    }

    
    return 0;
} 

*/
