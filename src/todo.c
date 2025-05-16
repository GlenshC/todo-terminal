
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "gc_logs.h"
#include "gc_tokens.h"
#include "types.h"
#include "stream.h"
#include "todo_tree.h"
#include "todo.h"
#include "todo_cmp.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef GC_DEBUG
#define GC_PERFORMANCE_TEST
// #define GC_PERFORMANCE_ITERATIONS 1026
#endif
/* 
    first add terminal functionality
    
    todo add "title" "description"      //add a todo 
    -d DD/MM/YYYY       // when to do (date)
    --date DD/MM/YYYY
    -p (1-5)            // priority (scale 1-5)
    --priority (1-5)

    todo list                           //list all available
    togo random                         //get a random task
    todo remove                         //show list and prompt where to remove
    todo clear
*/

//type macro
#define TODO_MAX_TOKENS 32

//macro
#define targ_get(index, default) (((index) < argc) ? (argv[(index)]) : (default))
#define todo_cmd(cmd) strcmp(argv[1], cmd)
#define todo_cmdi(cmd, index) strcmp(argv[(index)], cmd)
#define isbounded(val, start, end)   (start <= val && val <= end)
#define isboundedx(val, start, end)   (start < val && val < end)

static void todo_cmd_add(TodoList *list, int argc, char *argv[]);
static void todo_cmd_clear();
static void todo_cmd_remove(TodoList *list);

static void todo_add(TodoList *list, char *title, char *description, TodoDate *tododate, unsigned int priority);
static void todo_stream_display_sorted(TodoList *list, unsigned int index);
static void todo_list(TodoList *list);
static void todo_sorting_arg(TodoList *list, int argc, char *argv[]);

void todo_CLI(int argc, char *argv[])
{
    TodoList list = {};

    if (todo_cmd("clear") == 0)
    {
        todo_cmd_clear();
        return;
    }
    else if (todo_cmd("help") == 0)
    {
        printf("help\n"); // add
        return;
    }

    todo_stream_read(&list);


#ifdef GC_PERFORMANCE_TEST
#ifdef _WIN32
    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);
#else
    clock_t start, end;
    double cpu_time_used;

    start = clock();
#endif
#endif

    // Code to measure
    #ifdef GC_PERFORMANCE_ITERATIONS
    for (volatile long i = 0; i < GC_PERFORMANCE_ITERATIONS; i++)
    #endif
    {
        if (todo_cmd("add") == 0)
        {
            todo_cmd_add(&list, argc, argv); // redesign on the add comand
        }
        else if (todo_cmd("rm") == 0)
        {
            todo_sorting_arg(&list, argc, argv);
            todo_stream_sort(&list, list.sortingFunc);

            todo_list(&list);
            todo_cmd_remove(&list);
        }
        else if (todo_cmd("list") == 0)
        {
            todo_sorting_arg(&list, argc, argv);
            todo_stream_sort(&list, list.sortingFunc);

            todo_list(&list);    
        }
        else if (todo_cmd("get") == 0)
        {
            printf("get\n"); // add
        }
        else if (todo_cmd("edit") == 0)
        {
            printf("edit\n"); // add
        }
        else 
        {
            printf("Wrong Usage\n");
        }
    }

#ifdef GC_PERFORMANCE_TEST
#ifdef _WIN32
    QueryPerformanceCounter(&end);
    double elapsed = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
    printf("Elapsed time: %lf seconds\n", elapsed);
#else
    end = clock();

    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time taken: %lf seconds\n", cpu_time_used);
#endif
#endif
    todo_stream_write(&list);
    todo_stream_free_todolist(&list);
}

void todo_GUI()
{
    GC_LOG("GUI");
}


// 0    1    2     3    4  5        6  7
// todo list -p

static void todo_cmd_clear()
{
    char buffer[16];
    printf("Are you sure about that? (Yes/no): ");
    fgets(buffer, 16, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    if (strcmp(buffer, "Yes") == 0)
    {
        remove("todo.todo");
        printf("Cleared Todos.\n");
    }
    else
    {
        printf("Aborting...\n");
    }
}

static void todo_cmd_remove(TodoList *list)
{
    char buffer[32];
    printf("\nEnter item ID to remove: ");
    fgets(buffer, 32, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    if (strcmp("q", buffer) == 0)
    {
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

#define ADD_ARG_TITLE 2
#define ADD_ARG_DESC  3
static void todo_cmd_add(TodoList *list, int argc, char *argv[])
{
    char *argtokens[TODO_MAX_TOKENS] = {};

    char *description = NULL;
    TodoDate date = {};
    unsigned int priority = 0;

    if (targ_get(ADD_ARG_TITLE, NULL) == NULL)
    {
        //todo_help("add"); // implement
        exit(EXIT_FAILURE);
    }
    
    
    for (int i = 3; i < argc; i+=2)
    {
        char *nextarg = targ_get(i+1, NULL);
        if (nextarg == NULL)
        {
            GC_LOG("wrong usage");
            exit(EXIT_FAILURE);
        }

        if(todo_cmdi("-t", i) == 0)
        {
            gc_tokenize(nextarg, "/", argtokens, TODO_MAX_TOKENS);
            date.day    = (int)strtoul(argtokens[0], NULL, 0);
            date.month  = (int)strtoul(argtokens[1], NULL, 0);
            date.year   = (int)strtoul(argtokens[2], NULL, 0);
        }
        else if(todo_cmdi("-p", i) == 0)
        {
            priority = strtoul(nextarg, NULL, 0);            
        }else if(todo_cmdi("-d",i) == 0)
        {
            description = nextarg;
        }
    }
    
    todo_add(list, argv[ADD_ARG_TITLE], description, &date, priority);
}

static void todo_add(TodoList *list, char *title, char *description, TodoDate *tododate, unsigned int priority)
{
    time_t raw_created = time(NULL);
    time_t raw_deadline;
    struct tm *created_tm = localtime(&raw_created);
    struct tm deadline_tm = *created_tm;

    if (priority > 5U)
    {
        priority = 5U;
    }

    int year_diff = (tododate->year) - (created_tm->tm_year + 1900);
    int mon_diff = (tododate->month) - (created_tm->tm_mon + 1);
    
    if (year_diff > 0)
    {
        deadline_tm.tm_year = tododate->year - 1900;
        deadline_tm.tm_mon = tododate->month - 1;
        deadline_tm.tm_mday = tododate->day;
    }
    else if (year_diff == 0)
    {
        if (mon_diff > 0)
        {
            deadline_tm.tm_mon = tododate->month - 1;
            deadline_tm.tm_mday = tododate->day;
        }
        else if (mon_diff == 0)
        {
            if (isbounded(tododate->day, (created_tm->tm_mday), 31))
            {
                deadline_tm.tm_mday = tododate->day;
            }
        }
    }
    raw_deadline = mktime(&deadline_tm);
    
    TodoT todotask = {};
    
    todotask.titleSize = strlen(title);
    todotask.title = title;

    if (description != NULL)
    {
        todotask.descSize = strlen(description);
        todotask.desc = description;

    } else 
    {
        todotask.descSize = 1;
        todotask.desc = "\0";
    }
    
    todotask.priority = priority;
    todotask.created = raw_created;
    todotask.deadline = raw_deadline;
    
    todo_stream_push(list, &todotask);
}


static void todo_list(TodoList *list)
{
    if (list->size == 0)
    {
        printf("No todos found :\'(\n");
        return;
    }
    printf("%4s %-20s %-40s %-8s %-16s\n", "ID", "Title", "Description", "Priority", "Deadline");
    todo_tree_display(list, list->sortedList, todo_stream_display_sorted);    
}

/* TODO TREE COMPARE */
static void todo_stream_display_sorted(TodoList *list, unsigned int index)
{
    char buffer[20];
    strftime(buffer, 20, "%x", localtime(&list->deadline[index]));
    printf("%4u %-20s %-40s %-8llu %-16s\n", index+1, list->title[index], list->desc[index], list->priority[index], buffer);
}
/* 
todo list -p
0    1    2
*/
static void todo_sorting_arg(TodoList *list, int argc, char *argv[])
{
    list->sortingFunc = todo_tree_defaultCompare;
    if (argc > 2)
    {
        if (todo_cmdi("-p", 2) == 0)
        {
            GC_LOG("priority sort\n");
            list->sortingFunc = todo_tree_priorityCompare;
        }
        else if (todo_cmdi("-d", 2) == 0)
        {
            GC_LOG("deadline sort\n");
            list->sortingFunc = todo_tree_deadlineCompare;
        }
        else if (todo_cmdi("-c", 2) == 0)
        {
            GC_LOG("created sort\n");
            list->sortingFunc = todo_tree_createdCompare;
        }
        else if (todo_cmdi("-t", 2) == 0)
        {
            GC_LOG("title sort\n");
            list->sortingFunc = todo_tree_titleCompare;
        }
    }


}
/* 

*/