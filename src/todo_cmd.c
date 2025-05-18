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

static pScore TODO_ENERGY_WEIGHTS[] = {
    60,  30, 70, 30, // Low Energy
    80,  70, 50, 10, // Med Energy
    90, 100, 30,  0  // High Energy
};

static char *TODO_QUADRANTS[] = {
    "Critical", //0
    "Growth", //1
    "Distraction", //2
    "Waste" //3
};

static pScore todo_get_todopressure(time_t deadline, time_t today);
static pScore todo_get_todoage(time_t created, time_t today);

static uint32_t xorshift32(uint32_t *state) ; // random number generator

void todo_add(TodoList *list, char *title, char *description, TodoDate *tododate, unsigned int priority)
{
    time_t raw_created = time(NULL);
    time_t raw_deadline = 0;
    if (tododate != NULL)
    {
        raw_deadline = todo_date_createtime(tododate, raw_created);
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
    printf("%-4s %-20s %-40s %-15s %-16s\n", "ID", "Title", "Description", "Priority", "Deadline");
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
            }
            else if (todo_cmdi("-d", i) == 0)
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

time_t todo_get_timeToday(void)
{
    return time(NULL);
}


void todo_get_randomAction(TodoList *list)
{
    uint32_t timeToday = (uint32_t)todo_get_timeToday();
    int randomNumber = xorshift32(&timeToday) % (TODO_STREAM_MAX_TODOS);

    uint32_t index = 0;
    size_t size = list->size;
    for (uint32_t i = 0; randomNumber > 0; i++)
    {
        index = i%(size);
        randomNumber -= list->priorityScore[index];
    }

    todo_stream_display_sorted(list, index);
}

void todo_get_bestAction(TodoList *list)
{
    unsigned int index = todo_tree_at(list, list->sortedList, 0);
    printf("%-4s %-20s %-40s %-15s %-16s\n", "ID", "Title", "Description", "Priority", "Deadline");
    todo_stream_display_sorted(list, index);
}

/* ===========================================
    PRIORITY SCORE
   ===========================================*/

void todo_compute_priorityScores(TodoList *list)
{
    pScore *scoreTable = todo_get_todouserenergy(0);
    time_t timeToday = time(NULL);
    for (size_t i = 0; i < list->size; i++)
    {
        list->priorityScore[i] = todo_get_priorityScore(list, i, scoreTable, timeToday);
    }
}

pScore todo_get_priorityScore(TodoList *list, unsigned int index, pScore *scoreTable, time_t timeToday)
{

    pScore score = scoreTable[list->priority[index]];
    if (list->deadline[index])
    {
        score += todo_get_todopressure(list->deadline[index], timeToday);
    }
    score += todo_get_todoage(list->created[index], timeToday);

    return score;
}

pScore *todo_get_todouserenergy(unsigned int energy)
{
    char buffer[20];
    if (energy == 0)
    {
        printf("Enter energy levels? (1 - LOW, 2 - MEDIUM, 3 - HIGH): ");
        todo_getinput(buffer, 20);
        energy = strtoul(buffer, NULL, 0);
    }

    switch(energy)
    {
        case 1: // low
        {
            return TODO_ENERGY_WEIGHTS;
        }
        case 2: // med
        {
            return TODO_ENERGY_WEIGHTS + 4;
        }
        default:
            return TODO_ENERGY_WEIGHTS + 8;
    }
}

/* ===========================================
    HELPER FUNCTIONS
   ===========================================*/
//

static uint32_t xorshift32(uint32_t *state) 
{
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return *state = x;
}



static void todo_stream_display_sorted(TodoList *list, unsigned int index)
{
    char buffer[20] = {};
    if (list->deadline[index])
    {
        todo_time_str(buffer, 20, list->deadline[index]);
    }
    printf("%4u %-20s %-40s %-15s %-16s\n", index+1, list->title[index], list->desc[index], TODO_QUADRANTS[list->priority[index]], buffer);
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


static pScore todo_get_todopressure(time_t deadline, time_t today)
{
    time_t timeLeft = (deadline - today) / (60*60);
    if (timeLeft < 1)
        return 50;
    else if (timeLeft < 6)
        return 30;
    else if (timeLeft < 24)
        return 20;
    else if (timeLeft < 72)
        return 10;
    
    return 0;
}

static pScore todo_get_todoage(time_t created, time_t today)
{
    time_t ageDay = (created - today) / (24*60*60);
    if (ageDay > 7)
        return 10;    
    else if (ageDay > 30)
        return 20;
    
    return 0;
}

/*  */
