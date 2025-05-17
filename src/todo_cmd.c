#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "gc_logs.h"
#include "gc_string.h"
#include "gc_tokens.h"
#include "stream.h"
#include "todo_cmp.h"
#include "todo_tree.h"

#include "todo_cmd.h"


static time_t todo_date_createtime(TodoDate *tododate, time_t todayRaw);
static void todo_stream_display_sorted(TodoList *list, unsigned int index);
static void todo_getinput(char *buffer, int maxCount);
static void todo_time_str(char *buffer, size_t maxCount, time_t time);

void todo_add(TodoList *list, char *title, char *description, TodoDate *tododate, unsigned int priority)
{
    time_t raw_created = time(NULL);
    time_t raw_deadline = todo_date_createtime(tododate, raw_created);

    if (priority > 5U)
    {
        priority = 5U;
    }
    
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


void todo_list(TodoList *list)
{
    if (list->size == 0)
    {
        printf("No todos found :\'(\n");
        return;
    }
    printf("%4s %-20s %-40s %-8s %-16s\n", "ID", "Title", "Description", "Priority", "Deadline");
    todo_tree_display(list, list->sortedList, todo_stream_display_sorted);    
}

#define emptyBuffer(buf) (buf[0] = '\0')
#define isempty(buffer) (buffer[0] == 0)
void todo_edit(TodoList *list, unsigned int index)
{
    if (list->size <= index)
    {
        return;
    }

    char *argtokens[TODO_MAX_TOKENS] = {};
    TodoDate date = {};
    char ftimeBuffer[20];
    char inputBuffer[1024];
    emptyBuffer(inputBuffer);

    printf("\nCurrent title: %s\n", list->title[index]);
    printf("Edit title (press Enter to keep unchanged): ");
    
    todo_getinput(inputBuffer, 1024);
    if (!isempty(inputBuffer))
    {
        list->titleSize[index] = GC_STR_strcpy(list->title[index], inputBuffer, 0, 1024);
        
    }
    emptyBuffer(inputBuffer);
    
    printf("\nCurrent description: %s\n", list->desc[index]);
    printf("Edit description (press Enter to keep unchanged): ");

    todo_getinput(inputBuffer, 1024);
    if (!isempty(inputBuffer))
    {
        list->descSize[index] = GC_STR_strcpy(list->desc[index], inputBuffer, 0, 1024);
    }
    emptyBuffer(inputBuffer);

    printf("\nCurrent priority: %hhu\n", list->priority[index]);
    printf("Edit priority {0-5} (press Enter to keep unchanged): ");

    todo_getinput(inputBuffer, 1024);
    if (!isempty(inputBuffer))
    {
        list->priority[index] = strtoull(inputBuffer, NULL, 0);
    }
    emptyBuffer(inputBuffer);

    todo_time_str(ftimeBuffer, 20, list->deadline[index]);

    printf("\nCurrent deadline (DD/MM/YYYY): %s\n", ftimeBuffer);
    printf("Edit deadline {DD/MM/YYYY} (press Enter to keep unchanged): ");

    todo_getinput(inputBuffer, 1024);
    if (!isempty(inputBuffer))
    {        
        gc_tokenize(inputBuffer, "/", argtokens, TODO_MAX_TOKENS);
        date.day    = (int)strtoul(argtokens[0], NULL, 0);
        date.month  = (int)strtoul(argtokens[1], NULL, 0);
        date.year   = (int)strtoul(argtokens[2], NULL, 0);

        list->deadline[index] = todo_date_createtime(&date, list->created[index]);
    }
}


void todo_sorting_arg(TodoList *list, int argc, char *argv[])
{
    list->sortingFunc = todo_tree_defaultCompare;
    if (argc > 2)
    {
        if (todo_cmdi("-p", 2) == 0)
        {
            list->sortingFunc = todo_tree_priorityCompare;
        }
        else if (todo_cmdi("-d", 2) == 0)
        {
            list->sortingFunc = todo_tree_deadlineCompare;
        }
        else if (todo_cmdi("-c", 2) == 0)
        {
            list->sortingFunc = todo_tree_createdCompare;
        }
        else if (todo_cmdi("-t", 2) == 0)
        {
            list->sortingFunc = todo_tree_titleCompare;
        }
    }
}


/* ===========================================
    HELPER FUNCTIONS
   ===========================================*/
static void todo_stream_display_sorted(TodoList *list, unsigned int index)
{
    char buffer[20];
    todo_time_str(buffer, 20, list->deadline[index]);
    printf("%4u %-20s %-40s %-8hhu %-16s\n", index+1, list->title[index], list->desc[index], list->priority[index], buffer);
}

static void todo_getinput(char *buffer, int maxCount)
{
    fgets(buffer, maxCount, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
}

static void todo_time_str(char *buffer, size_t maxCount, time_t time)
{
    strftime(buffer, maxCount, "%d/%m/%Y", localtime(&time));
}

static time_t todo_date_createtime(TodoDate *tododate, time_t todayRaw)
{
    int year_diff = 0, mon_diff = 0, day_diff = 0; 
    struct tm *today_tm = localtime(&todayRaw);
    struct tm result_tm = *today_tm;
        
    year_diff = (tododate->year) - (today_tm->tm_year + 1900);
    mon_diff = (tododate->month) - (today_tm->tm_mon + 1);
    day_diff = (tododate->day) - (today_tm->tm_mday);
    
    if (year_diff > 0)
    {
        result_tm.tm_year = tododate->year - 1900;
        result_tm.tm_mon = tododate->month - 1;
        result_tm.tm_mday = tododate->day;
    }
    else if (year_diff == 0)
    {
        if (mon_diff > 0)
        {
            result_tm.tm_mon = tododate->month - 1;
            result_tm.tm_mday = tododate->day;
        }
        else if (mon_diff == 0)
        {
            if (isbounded(tododate->day, (today_tm->tm_mday), 31))
            {
                if (day_diff > 0)
                {
                    result_tm.tm_mday = tododate->day;
                }
            }
        }
    }
    return mktime(&result_tm);
}