#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "gc_logs.h"
#include "gc_tokens.h"
#include "types.h"
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
static void todo_add(const char *title, const char *description, TodoDate *date, unsigned int priority);

void todo_CLI(int argc, char *argv[])
{
    if (todo_cmd("add") == 0)
    {
        todo_add_cmd(argc, argv);
    }
    else if (todo_cmd("list") == 0)
    {
        GC_LOG("list\n");
        
    }
    else if (todo_cmd("get") == 0)
    {
        GC_LOG("get\n");
        
    }
    else if (todo_cmd("rm") == 0)
    {
        GC_LOG("remove\n");
        
    }
}

void todo_GUI()
{
    GC_LOG("GUI");
}


// 0    1   2     3    4  5        6  7
// todo add title desc -d 11/22/33 -p 1
#define ADD_ARG_TITLE 1
#define ADD_ARG_DESC  2
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
    description = targ_get(ADD_ARG_DESC, NULL);
    
    for (int i = 4; i < argc; i+=2)
    {
        char *nextarg = targ_get(i+1, NULL);
        if (nextarg == NULL)
            exit(EXIT_FAILURE);

        if(todo_cmdi("-d", i) == 0)
        {
            gc_tokenize(nextarg, "/", argtokens, TODO_MAX_TOKENS);
            date.day    = (int)strtoul(argtokens[0], NULL, 0);
            date.month  = (int)strtoul(argtokens[1], NULL, 0);
            date.year   = (int)strtoul(argtokens[2], NULL, 0);
        }
        else if(todo_cmdi("-p", i) == 0)
        {
            priority = strtoul(nextarg, NULL, 0);            
        }
    }

    todo_add(argv[ADD_ARG_TITLE], description, &date, priority);
}

static void todo_add(const char *title, const char *description, TodoDate *tododate, unsigned int priority)
{
    time_t rawtime = time(NULL);
    time_t todotime;
    struct tm *today = localtime(&rawtime);
    struct tm date = *today;

    if (isbounded(tododate->day, (today->tm_mday), 31))
    {
        date.tm_mday = tododate->day;
        if (isbounded(tododate->month, (today->tm_mon + 1), 11))
        {
            date.tm_mon = tododate->month - 1;
            
            if ((tododate->year) >= (today->tm_year+1900))
            {
                tododate->year - 1900;
            }
        }
    }
    

    if (description == NULL) description = "";
    
    printf( "title: %s\n"
            "descr: %s\n"
            "ddate: %u/%u/%u\n"
            "prior: %u\n"
            "cdate: %d/%d/%d %d:%d:%d\n",
            title, description,
            tododate->day, tododate->month, tododate->year,
            priority,
            today->tm_mday,
            today->tm_mon,
            today->tm_year,
            today->tm_hour,
            today->tm_min,
            today->tm_sec);
}