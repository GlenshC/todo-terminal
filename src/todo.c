
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "gc_logs.h"
#include "gc_tokens.h"
#include "types.h"
#include "stream.h"
#include "todo.h"

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

static void todo_add_cmd(int argc, char *argv[]);
static void todo_list_cmd();
static void todo_clear_cmd();

static void todo_add(char *title, char *description, TodoDate *tododate, unsigned int priority);

void todo_CLI(int argc, char *argv[])
{

    if (todo_cmd("add") == 0)
    {
        todo_add_cmd(argc, argv);
    }
    else if (todo_cmd("list") == 0)
    {
        todo_list_cmd();
    }
    else if (todo_cmd("clear") == 0)
    {
        todo_clear_cmd();
    }
    else if (todo_cmd("get") == 0)
    {
        GC_LOG("get\n");
    }
    else if (todo_cmd("rm") == 0)
    {
        GC_LOG("remove\n");
        
    }
    else if (todo_cmd("help") == 0)
    {
        GC_LOG("help\n");
    }
    else 
    {
        GC_LOG("Wrong Usage\n");
    }
}

void todo_GUI()
{
    GC_LOG("GUI");
}


// 0    1   2     3    4  5        6  7
// todo add title desc -d 11/22/33 -p 1
#define ADD_ARG_TITLE 2
#define ADD_ARG_DESC  3
static void todo_add_cmd(int argc, char *argv[])
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

    todo_add(argv[ADD_ARG_TITLE], description, &date, priority);
}

static void todo_add(char *title, char *description, TodoDate *tododate, unsigned int priority)
{
    time_t raw_created = time(NULL);
    time_t raw_deadline;
    struct tm *created_tm = localtime(&raw_created);
    struct tm deadline_tm = *created_tm;

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
        todotask.desc = "";
    }
    
    todotask.priority = priority;
    todotask.created = raw_created;
    todotask.deadline = raw_deadline;
    
    todo_stream_write(&todotask);

}


static void todo_list_cmd()
{
    TodoList list;
    char buffer[1024];
    
    if (todo_stream_read(&list))
    {
        GC_LOG("file not found.\n");
        return;
    }
    if (list.size == 0)
    {
        GC_LOG("No items\n");
        return;
    }
    
    printf("Title/Description/Priority/Deadline\n");
    for (size_t i = 0; i < list.size; i++)
    {
        strftime(buffer, 1024, "%x", localtime(&list.deadline[i]));
        printf("%s %s %llu %s\n", list.title[i], list.desc[i], list.priority[i], buffer);
    }

    todo_stream_free_todolist(&list);
}

static void todo_clear_cmd()
{
    char buffer[16];
    printf("Are you sure about that? (Yes/no): ");
    fgets(buffer, 16, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    if (strcmp(buffer, "Yes") == 0)
    {
        printf("Cleared Todos.\n");
        remove("todo.todo");
    }
    else
    {
        printf("Aborting...\n");
    }
}
/* 

*/