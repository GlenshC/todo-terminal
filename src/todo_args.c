#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "gc_tokens.h"
#include "gc_string.h"
#include "todo_help.h"
#include "todo_cmd.h"
#include "terminal_colors.h"
#include "todo_cmp.h"
#include "stream.h"

#include "todo_args.h"


#define ADD_ARG_TITLE 2
#define ADD_ARG_DESC  3

void todo_cmd_add(TodoList *list, int argc, char *argv[])
{
    char *argtokens[TODO_MAX_TOKENS] = {};

    char *description = NULL;
    TodoDate date = {};
    TodoDate *dateptr = NULL;
    unsigned int priority = 1;

    if (targ_get(ADD_ARG_TITLE, NULL) == NULL)
    {
        todo_help_command(COMMAND_ADD);
        exit(EXIT_FAILURE);
    }
    
    
    for (int i = 3; i < argc; i++)
    {
        char *nextarg = targ_get(i+1, NULL);

        if(todo_cmdi("-D", i) == 0)
        {
            dateptr = &date;
            gc_tokenize(nextarg, "/", argtokens, TODO_MAX_TOKENS);
            date.day    = (int)strtoul(argtokens[0], NULL, 0);
            date.month  = (int)strtoul(argtokens[1], NULL, 0);
            date.year   = (int)strtoul(argtokens[2], NULL, 0);
        }
        else if(todo_cmdi("-p", i) == 0)
        {
            if (isdigit(nextarg[0]))
            {
                priority = strtoul(nextarg, NULL, 0);
                if (priority == 0)
                {
                    priority = 1;
                }
                else {
                    priority -= 1;
                }
            }
            else
            {
                if (gc_str_partialMatch("critical", nextarg) == 0)
                {
                    priority = 0;
                }
                else if (gc_str_partialMatch("growth", nextarg) == 0)
                {
                    priority = 1;
                }
                else if (gc_str_partialMatch("distraction", nextarg) == 0)
                {
                    priority = 2;
                }
                else if (gc_str_partialMatch("waste", nextarg) == 0)
                {
                    priority = 3;
                }
            }
        }else if(todo_cmdi("-d",i) == 0)
        {
            description = nextarg;
        }
    }
    // 1 2 3 4
    // 0 1 2 3

    todo_add(list, argv[ADD_ARG_TITLE], description, dateptr, priority);
}

void todo_sorting_arg(TodoList *list, int argc, char *argv[])
{
    list->sortingFunc = todo_tree_defaultCompare;
    for (int i = 2; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] == '-')
        {
            if (todo_cmdi("--priority", i) == 0)
            {
                list->sortingFunc = todo_tree_priorityCompare;
            }
            else if (todo_cmdi("--best", i) == 0)
            {
                list->sortingFunc = todo_tree_priorityScoreCompare;
                list->energy = todo_get_args(argc, argv, i+1);
            }
            else if (todo_cmdi("--deadline", i) == 0)
            {
                list->sortingFunc = todo_tree_deadlineCompare;
            }
            else if (todo_cmdi("--created", i) == 0)
            {
                list->sortingFunc = todo_tree_createdCompare;
            }
            else if (todo_cmdi("--title", i) == 0)
            {
                list->sortingFunc = todo_tree_titleCompare;
            }
        }
        else if (argv[i][0] == '-')
        {
            if (todo_cmdi("-p", i) == 0)
            {
                list->sortingFunc = todo_tree_priorityCompare;
            }
            else if (todo_cmdi("-b", i) == 0)
            {
                list->sortingFunc = todo_tree_priorityScoreCompare;
                list->energy = todo_get_args(argc, argv, i+1);
            }
            else if (todo_cmdi("-D", i) == 0)
            {
                list->sortingFunc = todo_tree_deadlineCompare;
            }
            else if (todo_cmdi("-c", i) == 0)
            {
                list->sortingFunc = todo_tree_createdCompare;
            }
            else if (todo_cmdi("-t", i) == 0)
            {
                list->sortingFunc = todo_tree_titleCompare;
            }
        }
        
    }

}


void todo_cmd_edit(TodoList *list)
{
    if (list == NULL)
    {
        return;
    }

    char buffer[32];
    printf("\nEnter item ID to " TERMINAL_BOLD_YELLOW "edit: " TERMINAL_COLOR_RESET);
    fgets(buffer, 32, stdin);
    buffer[strcspn(buffer, "\n")] = 0;

    if (strcmp("q", buffer) == 0)
    {
        todo_stream_write(list);
        exit(EXIT_SUCCESS);
    }
    
    size_t index = 0;
    index = strtoull(buffer, NULL, 0);

    if (index == 0)
    {
        printf(TERMINAL_COLOR_RED "Invalid ID.\n" TERMINAL_COLOR_RESET);
        return;
    }
    
    todo_edit(list, index-1);
    printf("\n");
}

void todo_cmd_remove(TodoList *list)
{
    if (list == NULL)
    {
        return;
    }

    char buffer[32];
    printf("\nEnter item ID to " TERMINAL_BOLD_RED "remove: " TERMINAL_COLOR_RESET);
    fgets(buffer, 32, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    if (strcmp("q", buffer) == 0)
    {
        todo_stream_write(list);
        exit(EXIT_SUCCESS);
    }
    size_t index = 0;
    index = strtoull(buffer, NULL, 0);

    if (index == 0)
    {
        printf("Invalid ID.\n");
        return;
    }
    
    todo_stream_remove(list, index-1);
    printf("\n");
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
                remove("todo.todo");
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

    printf("Are you sure? [Yes/no]: ");
    fgets(buffer, 16, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    if (strcmp(buffer, "Yes") == 0)
    {
        remove("todo.todo");
        printf(TERMINAL_COLOR_GREEN "Cleared Todos.\n" TERMINAL_COLOR_RESET);
    }
    else
    {
        printf(TERMINAL_COLOR_RED "Aborting...\n" TERMINAL_COLOR_RESET);
    }
}