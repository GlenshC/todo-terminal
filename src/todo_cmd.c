#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "gc_logs.h"
#include "gc_string.h"
#include "gc_tokens.h"
#include "stream.h"
#include "todo_tree.h"
#include "terminal_colors.h"
#include "todoio.h"
#include "todo_error.h"
#include "todo_args.h"
#include "todo_get_alg.h"

#include "todo_cmd.h"

#define UCLAMP(val, max) (((val) > (max)) ? (max) : (val))
#define todo_display_table_header() printf(TERMINAL_COLOR_DIM "%-4s  %-20s %-40s %-15s %-16s\n" TERMINAL_COLOR_RESET, "ID" , "Title", "Description", "Priority", "Deadline")
#define todo_display_table_debug_header() printf(TERMINAL_COLOR_DIM "%-4s  %-20s %-40s %-15s %-16s PScore\n" TERMINAL_COLOR_RESET, "ID", "Title", "Description", "Priority", "Deadline")

static time_t todo_date_createtime(TodoDate *tododate, time_t todayRaw);
static void todo_stream_display_sorted(TodoList *list, unsigned int index);
static void todo_time_str(char *buffer, size_t maxCount, time_t time);
static void todo_display_table_list(TodoList *list);
static unsigned int todo_get_deadlineColor(time_t deadline, time_t today);

static char *TODO_QUADRANTS_COLOR[] = {
    TERMINAL_BOLD_RED,
    TERMINAL_BOLD_GREEN,
    TERMINAL_BOLD_BLUE,
    TERMINAL_BOLD_GRAY,
};
static char *TODO_COLORED_QUADRANTS[] = {
    TERMINAL_BOLD_RED "Critical" TERMINAL_COLOR_RESET,
    TERMINAL_BOLD_GREEN "Growth" TERMINAL_COLOR_RESET,
    TERMINAL_BOLD_BLUE "Distraction" TERMINAL_COLOR_RESET,
    TERMINAL_COLOR_BOLD "Waste" TERMINAL_COLOR_RESET
};

static char *TODO_QUADRANTS[] = {
    "Critical",     // Red Bold
    "Growth",       // Green Bold
    "Distraction",  // Blue Bold
    "Waste"         // Bright Gray Bold
};

static int TODO_DEADLINE_COLOR[] = {
    196, 202, 208, 214, 220, 226, 229, 250
};

time_t todo_get_timeToday(void)
{
    return time(NULL);
}

void todo_add(TodoList *list, char *title, char *description, TodoDate *tododate, uint8_t priority)
{
    time_t raw_created = time(NULL);
    time_t raw_deadline = 0;
    size_t title_size = 0, desc_size = 0;
    if (tododate != NULL)
    {
        raw_deadline = todo_date_createtime(tododate, raw_created);
    }

    TodoT todotask = {};
    
    
    title_size = strlen(title);
    todotask.titleSize = UCLAMP(title_size, FIELD_TITLE_MAX);
    todotask.title = title;
    
    if (description != NULL)
    {
        desc_size = strlen(description);
        todotask.descSize = UCLAMP(desc_size, FIELD_DESCRIPTION_MAX);
        todotask.desc = description;
        // GC_LOG("descSize %hhu, str = %s\n", todotask.descSize, description);

    } else 
    {
        todotask.desc = "";
        todotask.descSize = 0;
    }
    todotask.priority = priority;
    todotask.created = raw_created;
    todotask.deadline = raw_deadline;
    
    todo_stream_push(list, &todotask);
}

void todo_list(TodoList *list)
{
    if (list->size < 1)
    {
        todo_error(TODO_ERROR_EMPTY);
        return;
    }
    todo_display_table_list(list);
}



#define emptyBuffer(buf) (buf[0] = '\0')
#define isempty(buffer) (buffer[0] == 0)
void todo_edit(TodoList *list, unsigned int index)
{
    if (index >= list->size)
    {
        todo_error(TODO_ERROR_INVALID_ID);
        return;
    }

    char *argtokens[TODO_MAX_TOKENS] = {};
    TodoDate date = {};
    char ftimeBuffer[20] = {};
    char inputBuffer[INPUT_MAX_BUFFERS] = {};
    emptyBuffer(inputBuffer);

    printf(TERMINAL_COLOR_DIM "\nCurrent title: " TERMINAL_COLOR_RESET TERMINAL_COLOR_LIGHT_YELLOW "%s\n" TERMINAL_COLOR_RESET, list->title[index]);
    printf(TERMINAL_COLOR_DIM "Edit title (press Enter to keep unchanged): " TERMINAL_COLOR_RESET);
    
    todo_getinput(inputBuffer, INPUT_MAX_BUFFERS);
    if (!isempty(inputBuffer))
    {
        list->titleSize[index] = GC_STR_strcpy(list->title[index], inputBuffer, 0, FIELD_TITLE_MAX);
        
    }
    emptyBuffer(inputBuffer);
    
    printf(TERMINAL_COLOR_DIM "\nCurrent description: " TERMINAL_COLOR_RESET TERMINAL_COLOR_LIGHT_YELLOW "%s\n" TERMINAL_COLOR_RESET, list->desc[index]);
    printf(TERMINAL_COLOR_DIM "Edit description (press Enter to keep unchanged): " TERMINAL_COLOR_RESET);

    todo_getinput(inputBuffer, INPUT_MAX_BUFFERS);
    if (!isempty(inputBuffer))
    {
        list->descSize[index] = GC_STR_strcpy(list->desc[index], inputBuffer, 0, FIELD_DESCRIPTION_MAX);
    }
    emptyBuffer(inputBuffer);

    uint8_t priority = todo_getBit(list->priority, index, TODO_PRIORITY_BITS);

    printf(TERMINAL_COLOR_DIM "\nCurrent priority: " TERMINAL_COLOR_RESET "%s(%s) %hhu\n" TERMINAL_COLOR_RESET, TODO_QUADRANTS_COLOR[priority], TODO_QUADRANTS[priority], priority);
    printf(TERMINAL_COLOR_DIM "Edit priority [%s, %s, %s, %s | 1 - 4] \n" TERMINAL_COLOR_DIM "(press Enter to keep unchanged): " TERMINAL_COLOR_RESET, TODO_COLORED_QUADRANTS[0], TODO_COLORED_QUADRANTS[1], TODO_COLORED_QUADRANTS[2], TODO_COLORED_QUADRANTS[3]);

    todo_getinput(inputBuffer, INPUT_MAX_BUFFERS);
    if (!isempty(inputBuffer))
    {
        todo_writeBit(list->priority, todo_priority_input(inputBuffer, priority), index, TODO_PRIORITY_BITS);
    }
    emptyBuffer(inputBuffer);

    if (list->deadline[index])
    {
        todo_time_str(ftimeBuffer, 20, list->deadline[index] * SECONDS_IN_DAY);
    }

    printf(TERMINAL_COLOR_DIM "\nCurrent deadline (DD/MM/YYYY): " TERMINAL_COLOR_RESET TERMINAL_COLOR_LIGHT_YELLOW "%s\n" TERMINAL_COLOR_RESET, (list->deadline[index]) ? ftimeBuffer : "N/A");
    printf(TERMINAL_COLOR_DIM "Edit deadline {DD/MM/YYYY} (press Enter to keep unchanged): " TERMINAL_COLOR_RESET);

    todo_getinput(inputBuffer, INPUT_MAX_BUFFERS);
    if (!isempty(inputBuffer))
    {
        if (isdigit(inputBuffer[0]))
        {
            gc_tokenize(inputBuffer, "/", argtokens, TODO_MAX_TOKENS);
            date.day    = (int)strtoul(argtokens[0], NULL, 0);
            date.month  = (int)strtoul(argtokens[1], NULL, 0);
            date.year   = (int)strtoul(argtokens[2], NULL, 0);

            list->deadline[index] = todo_date_createtime(&date, list->timeToday);
        }
    }
}

void todo_view(TodoList *list, unsigned int index)
{
    if (index >= list->size)
    {
        printf(TERMINAL_BOLD_RED "Error: Invalid ID\n" TERMINAL_COLOR_RESET);
        return;
    }
    char deadline[20] = {};
    char created[20] = {};
    if (list->deadline[index])
    {
        todo_time_str(deadline, 20, list->deadline[index]);
    }
    todo_time_str(created, 20, list->created[index]);
    uint8_t priority = todo_getBit(list->priority, index, TODO_PRIORITY_BITS);
    printf(
        "\x1b[38;5;245m" "ID: %u\n" TERMINAL_COLOR_RESET
        "\x1b[38;5;245m" "Title: " TERMINAL_COLOR_RESET "\x1b[38;5;229m" "%s\n" TERMINAL_COLOR_RESET
        "\x1b[38;5;245m" "Description: " TERMINAL_COLOR_RESET "\x1b[38;5;252m" "%s\n" TERMINAL_COLOR_RESET
        "\x1b[38;5;245m" "Priority: " TERMINAL_COLOR_RESET "%s%s\n" TERMINAL_COLOR_RESET
        "\x1b[38;5;245m" "Deadline: " TERMINAL_COLOR_RESET "\x1b[38;5;%dm%s\n" TERMINAL_COLOR_RESET
        "\x1b[38;5;245m" "Created: " TERMINAL_COLOR_RESET "\x1b[38;5;180m" "%s\n" TERMINAL_COLOR_RESET,
        index + 1,
        list->title[index],
        list->desc[index],
        TODO_QUADRANTS_COLOR[priority], TODO_QUADRANTS[priority],
        TODO_DEADLINE_COLOR[todo_get_deadlineColor(list->deadline[index], list->timeToday)], deadline,
        created
    );
}

void todo_cmd_random(TodoList *list)
{
    if (list->size < 1)
    {
        todo_error(TODO_ERROR_EMPTY);
        return;
    }
    uint32_t timeToday = (uint32_t)todo_get_timeToday();
    int randomNumber = ((uintptr_t)&timeToday + xorshift32(&timeToday)) % (TODO_STREAM_MAX_TODOS*2);
    
    uint32_t index = 0;
    size_t size = list->size;
    for (uint32_t i = 0; randomNumber > 0; i++)
    {
        index = i%(size);
        randomNumber -= list->priorityScore[index];
    }

    todo_stream_display_sorted(list, index);
}

void todo_cmd_get(TodoList *list)
{
    if (list->size < 1)
    {
        todo_error(TODO_ERROR_EMPTY);
        return;
    }
    // FIXME
    // unsigned int index = todo_tree_at(list, list->sortedList, 0);
    // todo_stream_display_sorted(list, index);
}

void todo_clear()
{
    if (remove(get_todo_file_path()))
    {
        printf(TERMINAL_COLOR_RED "Failed to clear todo list.\n" TERMINAL_COLOR_RESET);
        exit(EXIT_FAILURE);
    }
    
    printf(TERMINAL_COLOR_GREEN "Cleared Todos.\n" TERMINAL_COLOR_RESET);
    exit(EXIT_SUCCESS);
}

void todo_done(TodoList *list, unsigned int index)
{
    todo_writeBit(list->done, 1, index, TODO_DONE_BITS);
    printf("Item %u marked done.\n", index + 1);
}

void todo_undo(TodoList *list, unsigned int index)
{
    todo_writeBit(list->done, 0, index, TODO_DONE_BITS);
    printf("Item %u marked undone.\n", index + 1);
}


/* ===========================================
    HELPER FUNCTIONS
   ===========================================*/
//



static void todo_stream_display_sorted(TodoList *list, unsigned int index)
{
    char buffer[20] = {};
    if (list->deadline[index])
    {
        todo_time_str(buffer, 20, list->deadline[index] * SECONDS_IN_DAY);
    }
    uint8_t priority = todo_getBit(list->priority, index, TODO_PRIORITY_BITS);

    printf(
        "%4u  %s%-20.18s" TERMINAL_COLOR_RESET TERMINAL_COLOR_LIGHT_GRAY " %-40.38s " TERMINAL_COLOR_RESET "%s%-15.13s" TERMINAL_COLOR_RESET " \x1b[38;5;%dm%-16.14s\n" TERMINAL_COLOR_RESET, 
        index+1, 
        TODO_QUADRANTS_COLOR[priority], list->title[index], list->desc[index], 
        TODO_QUADRANTS_COLOR[priority], TODO_QUADRANTS[priority], 
        TODO_DEADLINE_COLOR[todo_get_deadlineColor(list->deadline[index], list->timeToday)], buffer);
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
    
    if (tododate->year)
    {
        year_diff = (tododate->year) - (today_tm->tm_year + 1900);
    }
    if (tododate->month)
    {
        mon_diff = (tododate->month) - (today_tm->tm_mon + 1);
    }
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
    result_tm.tm_min = 0;
    result_tm.tm_sec = 0;
    result_tm.tm_hour = 0;
    return mktime(&result_tm);
}

static unsigned int todo_get_deadlineColor(time_t deadline, time_t today)
{
/*    6   12   24   48   72  120  168 */
    float timeLeft = ((deadline*SECONDS_IN_DAY) - today) / (60.0f*60.0f);
    if (timeLeft < (MAX_HOURS * 0.05))
        return 0;
    else if (timeLeft < (MAX_HOURS * 0.1))
        return 1;
    else if (timeLeft < (MAX_HOURS * 0.5))
        return 2;
    else if (timeLeft < (MAX_HOURS * 0.65))
        return 3;
    else if (timeLeft < (MAX_HOURS * 0.8))
        return 4;
    else if (timeLeft < (MAX_HOURS))
        return 5;

    return 7;
} // 1970

static void todo_display_sortedlist(TodoList *list)
{
    for (size_t i = 0; i < list->size; i++)
    {
        todo_stream_display_sorted(list, list->sortedList[i]);
    }
}

static void todo_display_table_list(TodoList *list)
{
    todo_display_table_debug_header();
    todo_display_sortedlist(list);
}

