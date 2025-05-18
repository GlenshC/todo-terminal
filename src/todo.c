
#include <stdio.h>
#include <stdlib.h>
// #include <time.h>

#include "gc_logs.h"
#include "gc_string.h"
#include "gc_tokens.h"
#include "types.h"
#include "stream.h"
#include "todo_cmd.h"

#include "todo.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef GC_DEBUG
#define GC_PERFORMANCE_TEST
// #define GC_PERFORMANCE_ITERATIONS 1024
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

//macro

static void todo_cmd_add(TodoList *list, int argc, char *argv[]);
static void todo_cmd_clear(int argc, char *argv[]);
static void todo_cmd_remove(TodoList *list);
static void todo_cmd_edit(TodoList *list);
static void todo_cmd_get(TodoList *list, int israndom);
static unsigned int todo_get_args(int argc, char *argv[]);

void todo_CLI(int argc, char *argv[])
{
    TodoList list = {};

    if (todo_cmd("clear") == 0)
    {
        todo_cmd_clear(argc, argv);
        return;
    }
    else if (todo_cmd("help") == 0)
    {
        todo_help();
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
            todo_stream_sort(&list);

            todo_list(&list);
            todo_cmd_remove(&list);
        }
        else if (todo_cmd("list") == 0)
        {
            todo_sorting_arg(&list, argc, argv);
            todo_stream_sort(&list);

            todo_list(&list);    
        }
        else if (todo_cmd("get") == 0)
        {
            todo_stream_priorityScoreSort(&list, todo_get_args(argc, argv));
            
            todo_cmd_get(&list, 0);
        }
        else if (todo_cmd("random") == 0)
        {            
            todo_stream_priorityScoreSort(&list, todo_get_args(argc, argv));

            todo_cmd_get(&list, 1);
        }
        else if (todo_cmd("edit") == 0)
        {
            todo_sorting_arg(&list, argc, argv);
            todo_stream_sort(&list);
            
            todo_list(&list);    
            todo_cmd_edit(&list);
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
#ifdef GC_DEBUG
    todo_stream_free_todolist(&list); // remove if you want faster exit
#endif
}

void todo_GUI()
{
    GC_LOG("GUI");
}


// 0    1    2     3    4  5        6  7
// todo list -p

static void todo_cmd_edit(TodoList *list)
{
    if (list == NULL)
    {
        return;
    }

    char buffer[32];
    printf("\nEnter item ID to remove: ");
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
    
    todo_edit(list, index-1);
    printf("\n");
}

static void todo_cmd_clear(int argc, char *argv[])
{   
    // 1    2      3
    // 0    1      2
    // todo clear --force
    if (argc > 2){
        if (argv[2][0] == '-' && argv[2][1] == '-')
        {
            if (gc_str_strcmp("--force", argv[2]) == 0)
            {
                remove("todo.todo");
                return;
            }
        }
    }
    char buffer[16];

    printf("Are you sure? [Yes/no]: ");
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
    if (list == NULL)
    {
        return;
    }

    char buffer[32];
    printf("\nEnter item ID to remove: ");
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

#define ADD_ARG_TITLE 2
#define ADD_ARG_DESC  3
static void todo_cmd_add(TodoList *list, int argc, char *argv[])
{
    char *argtokens[TODO_MAX_TOKENS] = {};

    char *description = NULL;
    TodoDate date = {};
    TodoDate *dateptr = NULL;
    unsigned int priority = 0;

    if (targ_get(ADD_ARG_TITLE, NULL) == NULL)
    {
        //todo_help("add"); // implement
        exit(EXIT_FAILURE);
    }
    
    
    for (int i = 3; i < argc; i++)
    {
        char *nextarg = targ_get(i+1, NULL);

        if(todo_cmdi("-t", i) == 0)
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

static void todo_cmd_get(TodoList *list, int israndom)
{

    if (israndom)
    {
        todo_get_randomAction(list);
    }
    else
    {
        todo_get_bestAction(list);
    }

}

static unsigned int todo_get_args(int argc, char *argv[])
{
    if (argc > 2)
    {
        if (isdigit(argv[2][0]))
        {
            return strtoul(argv[2], NULL, 0);
        }
        else
        {
            if (gc_str_partialMatch("high", argv[2]) == 0)
            {
                return 3;
            }
            else if (gc_str_partialMatch("medium", argv[2]) == 0)
            {
                return 2;
            }
            else if (gc_str_partialMatch("low", argv[2]) == 0)
            {
                return 1;
            }
        }
    }
    return 0;
}


void todo_help(void)
{

}
/* 
    urgent? important? remove first
*/

/* ===========================================
    HELPER FUNCTIONS
   ===========================================*/