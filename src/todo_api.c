#include <stdlib.h>

#include "todo_api.h"
#include "todo_args.h"
#include "todo_cmd.h"
#include "stream_sort.h"
#include "stream.h"

#include "gc_speed_test.h"

static void todo_sort(TodoList *list, int argc, char *argv[]);
static void todo_prioritySort(TodoList *list, int argc, char *argv[]);

void cmd_clear(int argc, char *argv[])
{
    mk_speedtest(test);
    PerfomanceTime_Start(&test, "todo clear Test");
    
    todo_cmd_clear(argc, argv);
    
    PerfomanceTime_End(test);
    
    exit(EXIT_SUCCESS);
}

void cmd_add(TodoList *list, int argc, char *argv[])
{
    
    mk_speedtest(test);
    PerfomanceTime_Start(&test, "todo add Test");
    
    todo_cmd_add(list, argc, argv);
    
    PerfomanceTime_End(test);

    // todo_writelist(list);
}

void cmd_remove(TodoList *list, int argc, char *argv[])
{

    mk_speedtest(test);
    PerfomanceTime_Start(&test, "todo rm Test");
    
    todo_cmd_remove(list, argc, argv);
    
    PerfomanceTime_End(test);
    // todo_writelist(list);
}

void cmd_list(TodoList *list, int argc, char *argv[])
{

    todo_sort(list, argc, argv);
    
    mk_speedtest(test);
    PerfomanceTime_Start(&test, "todo list Test");
    
    todo_list(list);

    PerfomanceTime_End(test);
}

void cmd_get(TodoList *list, int argc, char *argv[])
{
    todo_prioritySort(list, argc, argv);
    
    mk_speedtest(test);
    PerfomanceTime_Start(&test, "todo get Test");
    
    todo_cmd_get(list);
    
    PerfomanceTime_End(test);
}

void cmd_random(TodoList *list, int argc, char *argv[])
{
    todo_prioritySort(list, argc, argv);

    mk_speedtest(test);
    PerfomanceTime_Start(&test, "todo random Test");
    
    todo_cmd_random(list);
    
    PerfomanceTime_End(test);
}

void cmd_edit(TodoList *list, int argc, char *argv[])
{

    mk_speedtest(test);
    PerfomanceTime_Start(&test, "todo edit Test");
    
    todo_cmd_edit(list, argc, argv);
    
    PerfomanceTime_End(test);
    // todo_writelist(list);
}

void cmd_view(TodoList *list, int argc, char *argv[])
{

    mk_speedtest(test);
    PerfomanceTime_Start(&test, "todo view Test");
    
    todo_cmd_view(list, argc, argv);
    
    PerfomanceTime_End(test);
}

void cmd_done(TodoList *list, int argc, char *argv[])
{

    mk_speedtest(test);
    PerfomanceTime_Start(&test, "todo done Test");
    
    todo_cmd_done(list, argc, argv); // implement
    
    PerfomanceTime_End(test);
    
    // todo_writelist(list);
}

void cmd_undo(TodoList *list, int argc, char *argv[])
{
    mk_speedtest(test);
    PerfomanceTime_Start(&test, "todo undo Test");
    
    todo_cmd_undo(list, argc, argv); //implement
    
    PerfomanceTime_End(test);    
    
    // todo_writelist(list);
}

void todo_readlist(TodoList *list)
{
    mk_speedtest(test);
    PerfomanceTime_Start(&test, "Read Test");
    
    todo_stream_read(list);
    
    PerfomanceTime_End(test);
}

void todo_writelist(TodoList *list)
{
    mk_speedtest(writetest);
    PerfomanceTime_Start(&writetest, "Write Test");

    todo_stream_write(list);

    PerfomanceTime_End(writetest);
}

void todo_freelist(TodoList *list)
{
    mk_speedtest(test);
    PerfomanceTime_Start(&test, "Free Resource Test");
    
    todo_stream_free_todolist(list);
    
    PerfomanceTime_End(test);
}

/* ====================================
    PRIVATE
   ==================================== */
static void todo_sort(TodoList *list, int argc, char *argv[])
{
    mk_speedtest(sorttest);
    PerfomanceTime_Start(&sorttest, "Sort Test");

    todo_sorting_arg(list, argc, argv);
    todo_sort_stream(list);

    PerfomanceTime_End(sorttest);
}

static void todo_prioritySort(TodoList *list, int argc, char *argv[])
{
    todo_cmd_prioritySort_args(list, argc, argv);
    todo_sort_stream(list);
}