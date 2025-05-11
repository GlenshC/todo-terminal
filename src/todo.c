
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "gc_logs.h"
#include "gc_tokens.h"
#include "types.h"
#include "file.h"
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
            exit(EXIT_FAILURE);

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

static void todo_add(const char *title, const char *description, TodoDate *tododate, unsigned int priority)
{
    time_t raw_time = time(NULL);
    time_t todo_time;
    struct tm *today_tm = localtime(&raw_time);
    struct tm date_tm = *today_tm;
    int year_diff = (tododate->year) - (today_tm->tm_year + 1900);
    int mon_diff = (tododate->month) - (today_tm->tm_mon + 1);
    // int day_diff = (tododate->day) - (today_tm->tm_mday);
    
    if (year_diff > 0)
    {
        date_tm.tm_year = tododate->year - 1900;
        date_tm.tm_mon = tododate->month - 1;
        date_tm.tm_mday = tododate->day;
    }
    else if (year_diff == 0)
    {
        if (mon_diff > 0)
        {
            date_tm.tm_mon = tododate->month - 1;
            date_tm.tm_mday = tododate->day;
        }
        else if (mon_diff == 0)
        {
            if (isbounded(tododate->day, (today_tm->tm_mday), 31))
            {
                date_tm.tm_mday = tododate->day;
            }
        }
    }
    todo_time = mktime(&date_tm);

    FILE *file = todo_file();

    if (description == NULL)
    {
        todo_fprintf(
            file, 
            "\"%s\"," // title
            "0," // desc
            "%u,"   // raw time deadline
            "%lld,"   // raw time deadline
            "%lld\n",   // raw time today
            title,
            priority,
            todo_time,
            raw_time
        );
        
    }
    else
    {
        todo_fprintf(
            file, 
            "\"%s\"," // title
            "\"%s\"," // desc
            "%u," // desc
            "%lld,"   // raw time deadline
            "%lld\n",   // raw time today
            title,
            description,
            priority,
            todo_time,
            raw_time
        );
    }

    int a;
    float b;
    char c;

    fread(&a, sizeof(a), 1, file);
    fread(&b, sizeof(b), 1, file);
    fread(&c, sizeof(c), 1, file);
        
        
    todo_file_close();

}
/* 

*/