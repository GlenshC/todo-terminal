#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "gc_logs.h"
#include "gc_tokens.h"
#include "gc_string.h"
#include "todo_help.h"
#include "todo_cmd.h"
#include "terminal_colors.h"
#include "todo_cmp.h"
#include "stream.h"
#include "todo_error.h"

#include "todoio.h"

#include "todo_args.h"


static char *todo_cmd_add_arg_desciription(int index, int argc, char *argv[]);
static uint8_t todo_cmd_add_arg_priority(int index, int argc, char *argv[]);
static TodoDate *todo_cmd_add_arg_deadline(TodoDate *date, int index, int argc, char *argv[]);
static size_t todo_inputItemID(TodoList * list, size_t item_id, const char *displayText);

#define ADD_ARG_TITLE 2
// #define ADD_ARG_DESC  3
void todo_cmd_add(TodoList *list, int argc, char *argv[])
{
    char *description = NULL;
    TodoDate date = {};
    TodoDate *dateptr = NULL;
    uint8_t priority = 1;

    if (targ_get(ADD_ARG_TITLE, NULL) == NULL)
    {
        todo_help_command(COMMAND_ADD);
        exit(EXIT_FAILURE);
    }
    if (strlen(argv[ADD_ARG_TITLE]) < 1)
    {
        todo_error(TODO_ERROR_ADD_TITLEARG_EMPTY);
        exit(EXIT_FAILURE);
    }
    
    for (int i = 3; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (argv[i][1] == '-')
            {
                if(todo_cmdi("--deadline", i) == 0)
                {
                    dateptr = todo_cmd_add_arg_deadline(&date, i++, argc, argv);
                }
                else if(todo_cmdi("--priority", i) == 0)
                {
                    priority = todo_cmd_add_arg_priority(i++, argc, argv);
                }
                else if(todo_cmdi("--description",i) == 0)
                {
                    description = todo_cmd_add_arg_desciription(i++, argc, argv);
                }

            }
            else
            {
                if(todo_cmdi("-D", i) == 0)
                {
                    dateptr = todo_cmd_add_arg_deadline(&date, i++, argc, argv);
                }
                else if(todo_cmdi("-p", i) == 0)
                {
                    priority = todo_cmd_add_arg_priority(i++, argc, argv);
                }
                else if(todo_cmdi("-d",i) == 0)
                {
                    description = todo_cmd_add_arg_desciription(i++, argc, argv);
                }
            }
        }
        
    }
    todo_add(list, argv[ADD_ARG_TITLE], description, dateptr, priority);
}

void todo_sorting_arg(TodoList *list, int argc, char *argv[])
{
    list->sortingFunc = TODO_SORT_DEFAULT;
    for (int i = 2; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            continue;
        } //--sort

        if (argv[i][1] == '-')
        {
            
            if (todo_cmdi("--sort", i) == 0)
            {
                if (gc_str_partialMatch("priority", targ_get(i+1, NULL)) == 0)
                {
                    list->sortingFunc = TODO_SORT_PRIORITY;
                }
                else if (gc_str_partialMatch("best", targ_get(i+1, NULL)) == 0)
                {
                    list->sortingFunc = TODO_SORT_BEST;
                    list->energy = todo_energy_option(argc, argv);
                }
                else if (gc_str_partialMatch("deadline", targ_get(i+1, NULL)) == 0)
                {
                    list->sortingFunc = TODO_SORT_DEADLINE;
                }
                else if (gc_str_partialMatch("created", targ_get(i+1, NULL)) == 0)
                {
                    list->sortingFunc = TODO_SORT_CREATED;
                }
                else if (gc_str_partialMatch("title", targ_get(i+1, NULL)) == 0)
                {
                    list->sortingFunc = TODO_SORT_TITLE;
                }                
            }
            else if(todo_cmdi("--reverse", i) == 0)
            {
                list->isAccending = -1;
            }
        }
        else
        {
            if (todo_cmdi("-s", i) == 0)
            {
                if (gc_str_partialMatch("p", targ_get(i+1, NULL)) == 0)
                {
                    list->sortingFunc = TODO_SORT_PRIORITY;
                }
                else if (gc_str_partialMatch("b", targ_get(i+1, NULL)) == 0)
                {
                    list->sortingFunc = TODO_SORT_BEST;
                    list->energy = todo_energy_option(argc, argv);
                }
                else if (gc_str_partialMatch("d", targ_get(i+1, NULL)) == 0)
                {
                    list->sortingFunc = TODO_SORT_DEADLINE;
                }
                else if (gc_str_partialMatch("c", targ_get(i+1, NULL)) == 0)
                {
                    list->sortingFunc = TODO_SORT_CREATED;
                }
                else if (gc_str_partialMatch("t", targ_get(i+1, NULL)) == 0)
                {
                    list->sortingFunc = TODO_SORT_TITLE;
                }                

            }
            else if(todo_cmdi("-r", i) == 0)
            {
                list->isAccending = -1;
            }
            
        }
    }

}


void todo_cmd_edit(TodoList *list, int argc, char *argv[])
{
    if (list == NULL || list->size < 1)
    {
        todo_error(TODO_ERROR_EMPTY);
        return;
    }

    size_t item_id = 0;
    item_id = todo_arg_id(argc, argv, 2, list->size);
    
    if (item_id == 0)
    {
        todo_sorting_arg(list, argc, argv);
        todo_stream_sort(list);
        item_id = todo_inputItemID(list, item_id, "\nEnter item ID to " TERMINAL_BOLD_YELLOW "edit: " TERMINAL_COLOR_RESET);
    }
    
    todo_edit(list, item_id-1);
    printf("\n");
}

void todo_cmd_view(TodoList *list, int argc, char *argv[])
{
    if (list == NULL || list->size < 1)
    {
        todo_error(TODO_ERROR_EMPTY);
        return;
    }

    size_t item_id = 0;
    item_id = todo_arg_id(argc, argv, 2, list->size);
    // GC_LOG("item_id: %zu\n", item_id);

    if (item_id == 0)
    {
        todo_sorting_arg(list, argc, argv);
        todo_stream_sort(list);
        item_id = todo_inputItemID(list, item_id, "\nEnter item ID to " TERMINAL_BOLD_BLUE "view: " TERMINAL_COLOR_RESET);
    }
    todo_view(list, item_id-1);
    printf("\n");
}

void todo_cmd_remove(TodoList *list, int argc, char *argv[])
{
    if (list == NULL || list->size < 1)
    {
        todo_error(TODO_ERROR_EMPTY);        
        return;
    }
    
    size_t item_id = 0;
    item_id = todo_arg_id(argc, argv, 2, list->size);
    // 255 > 254 
    if (item_id == 0)
    {
        todo_sorting_arg(list, argc, argv);
        todo_stream_sort(list);
        item_id = todo_inputItemID(list, item_id, "\nEnter item ID to " TERMINAL_BOLD_RED "remove: " TERMINAL_COLOR_RESET);
    }

    todo_stream_remove(list, item_id-1);
    printf("\n");
}

void todo_cmd_clear(int argc, char *argv[])
{   
    // 1    2      3
    // 0    1      2
    // todo clear --force
    if (argc == 3){
        if (argv[2][0] == '-' && argv[2][1] == '-')
        {
            if (gc_str_strcmp("--force", argv[2]) == 0)
            {
                todo_clear();
                return;
            }
        }
        else
        {
            todo_help_command(COMMAND_CLEAR);
            exit(EXIT_FAILURE);
        }
    }
    
    char buffer[16];

    printf("Reset todo list? [Yes/no]: ");
    fgets(buffer, 16, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    if (strcmp(buffer, "Yes") == 0)
    {
        todo_clear();
    }
    else
    {
        printf(TERMINAL_COLOR_RED "Aborting...\n" TERMINAL_COLOR_RESET);
    }
}

void todo_cmd_done(TodoList *list, int argc, char *argv[])
{
    if (list == NULL || list->size < 1)
    {
        todo_error(TODO_ERROR_EMPTY);
        return;
    }

    size_t item_id = 0;
    item_id = todo_arg_id(argc, argv, 2, list->size);

    if (item_id == 0)
    {
        todo_sorting_arg(list, argc, argv);
        todo_stream_sort(list);
        item_id = todo_inputItemID(list, item_id, "\nEnter item ID to " TERMINAL_BOLD_YELLOW "mark done: " TERMINAL_COLOR_RESET);
    }
    
    todo_done(list, item_id-1);
}
void todo_cmd_undo(TodoList *list, int argc, char *argv[])
{
    if (list == NULL || list->size < 1)
    {
        todo_error(TODO_ERROR_EMPTY);
        return;
    }

    size_t item_id = 0;
    item_id = todo_arg_id(argc, argv, 2, list->size);

    if (item_id == 0)
    {
        todo_sorting_arg(list, argc, argv);
        todo_stream_sort(list);
        item_id = todo_inputItemID(list, item_id, "\nEnter item ID to " TERMINAL_BOLD_YELLOW "undo: " TERMINAL_COLOR_RESET);
    }
    
    todo_undo(list, item_id-1);

}


/* ===================================================================
    ARGSSSS 
   ===================================================================*/

size_t todo_arg_id(int argc, char *argv[], int argv_index, size_t max_val)
{
    if (argv_index >= argc)
    {
        return 0;
    }

    size_t val = strtoull(argv[argv_index], NULL, 0);
    
    if (val > max_val)
    {
        return 0;
    }
    
    return val;
}

unsigned int todo_get_args(int argc, char *argv[], unsigned int index)
{
    if (index >= (unsigned int)argc)
    {
        return 0;
    }
    if (argc > 2)
    {
        if (isdigit(argv[index][0]))
        {
            return strtoul(argv[index], NULL, 0);
        }
        else
        {
            if (gc_str_partialMatch("high", argv[index]) == 0)
            {
                return 3;
            }
            else if (gc_str_partialMatch("medium", argv[index]) == 0)
            {
                return 2;
            }
            else if (gc_str_partialMatch("low", argv[index]) == 0)
            {
                return 1;
            }
        }
    }
    return 0;
}

unsigned int todo_energy_option(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            continue;
        }
        if (argv[i][1] == '-')
        {
            if (todo_cmdi("--energy", i) == 0)
            {
                if (isdigit(argv[i+1][0]))
                {
                    return strtoul(argv[i+1], NULL, 0);
                }
                else
                {
                    if (gc_str_partialMatch("high", argv[i+1]) == 0)
                    {
                        return 3;
                    }
                    else if (gc_str_partialMatch("medium", argv[i+1]) == 0)
                    {
                        return 2;
                    }
                    else if (gc_str_partialMatch("low", argv[i+1]) == 0)
                    {
                        return 1;
                    }
                    else
                    {
                        return 0;
                    }
                }
            }
        }
        else
        {
            if (todo_cmdi("-E", i) == 0)
            {
                if (isdigit(argv[i+1][0]))
                {
                    return strtoul(argv[i+1], NULL, 0);
                }
                else
                {        
                    if (gc_str_partialMatch("high", argv[i+1]) == 0)
                    {
                        return 3;
                    }
                    else if (gc_str_partialMatch("medium", argv[i+1]) == 0)
                    {
                        return 2;
                    }
                    else if (gc_str_partialMatch("low", argv[i+1]) == 0)
                    {
                        return 1;
                    }
                    else
                    {
                        return 0;
                    }
                }
            }
        }  
    }
    return 0;
}

void todo_cmd_prioritySort_args(TodoList *list, int argc, char *argv[])
{
    list->energy = todo_get_args(argc, argv, 2);
    list->sortingFunc = TODO_SORT_BEST;
}

/* ======================================
   PRIVATE FUNCTIONS
   ====================================== */

static size_t todo_inputItemID(TodoList * list, size_t item_id, const char *displayText)
{
    if (item_id == 0)
    {
        todo_list(list);

        char buffer[32];
        printf(displayText);
        fgets(buffer, 32, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
    
        if (strcmp("q", buffer) == 0)
        {
            todo_stream_write(list);
            exit(EXIT_SUCCESS);
        }
        
        item_id = strtoull(buffer, NULL, 0);
        
        if (item_id == 0)
        {
            todo_error(TODO_ERROR_INVALID_ID);
            todo_stream_write(list);
            exit(EXIT_SUCCESS);
        }
    }
    return item_id;
}

static TodoDate *todo_cmd_add_arg_deadline(TodoDate *date, int index, int argc, char *argv[])
{
    char *argtokens[TODO_MAX_TOKENS] = {};
    char *nextarg = targ_get(index+1, NULL);
    if (nextarg == NULL)
    {
        todo_error(TODO_ERROR_INVALID_CMD_USAGE);
        exit(EXIT_FAILURE);
    }

    gc_tokenize(nextarg, "/", argtokens, TODO_MAX_TOKENS);
    if (isdigit(argtokens[0][0]))
    {
        date->day    = (int)strtoul(argtokens[0], NULL, 0);
        date->month  = (int)strtoul(argtokens[1], NULL, 0);
        date->year   = (int)strtoul(argtokens[2], NULL, 0);
        return date;
    }
    return NULL;
}

static uint8_t todo_cmd_add_arg_priority(int index, int argc, char *argv[])
{
    
    char *nextarg = targ_get(index+1, NULL);
    if (nextarg == NULL)
    {
        todo_error(TODO_ERROR_INVALID_CMD_USAGE);
        exit(EXIT_FAILURE);
    }
    
    return todo_priority_input(nextarg, 1);
}



static char *todo_cmd_add_arg_desciription(int index, int argc, char *argv[])
{
    char *nextarg = targ_get(index+1, NULL);

    if (nextarg == NULL)
    {
        todo_error(TODO_ERROR_INVALID_CMD_USAGE);
        exit(EXIT_FAILURE);
    }
    return nextarg;
}
// 0   1  2  3
// 00 01 10 11
uint8_t todo_priority_input(char *str, uint8_t defaultpriority) 
{
    uint8_t priority = defaultpriority;
    if (isdigit(str[0]))
    {
        priority = strtoul(str, NULL, 0);
        if (priority == 0)
        {
            priority = defaultpriority;
        }
        else {
            priority -= 1;
        }
    }
    else
    {
        if (gc_str_partialMatch("critical", str) == 0)
        {
            priority = 0;
        }
        else if (gc_str_partialMatch("growth", str) == 0)
        {
            priority = 1;
        }
        else if (gc_str_partialMatch("distraction", str) == 0)
        {
            priority = 2;
        }
        else if (gc_str_partialMatch("waste", str) == 0)
        {
            priority = 3;
        }
    }
    return priority;
}