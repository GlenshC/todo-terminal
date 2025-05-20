#include <stdlib.h>

#include "todo_api.h"
#include "todo_args.h"
#include "todo_cmd.h"
#include "stream.h"

static void todo_sort(TodoList *list, int argc, char *argv[]);
static void todo_prioritySort(TodoList *list, int argc, char *argv[]);

void cmd_clear(int argc, char *argv[])
{
    todo_cmd_clear(argc, argv);
    exit(EXIT_SUCCESS);
}

void cmd_add(TodoList *list, int argc, char *argv[])
{
    todo_cmd_add(list, argc, argv);
}

void cmd_remove(TodoList *list, int argc, char *argv[])
{
    todo_sort(list, argc, argv);

    todo_cmd_remove(list, argc, argv);
}

void cmd_list(TodoList *list, int argc, char *argv[])
{
    todo_sort(list, argc, argv);

    todo_list(list);
}

void cmd_get(TodoList *list, int argc, char *argv[])
{
    todo_prioritySort(list, argc, argv);
    todo_cmd_get(list);
}

void cmd_random(TodoList *list, int argc, char *argv[])
{
    todo_prioritySort(list, argc, argv);
    todo_cmd_random(list);
}

void cmd_edit(TodoList *list, int argc, char *argv[])
{
    todo_sort(list, argc, argv);

    todo_cmd_edit(list, argc, argv);
}

void cmd_view(TodoList *list, int argc, char *argv[])
{
    todo_sort(list, argc, argv);

    todo_cmd_view(list, argc, argv);
}

void todo_readlist(TodoList *list)
{
    todo_stream_read(list);
}

void todo_writelist(TodoList *list)
{
    todo_stream_write(list);
}

void todo_freelist(TodoList *list)
{
    todo_stream_free_todolist(list);
}

/* ====================================
    PRIVATE
   ==================================== */
static void todo_sort(TodoList *list, int argc, char *argv[])
{
    todo_sorting_arg(list, argc, argv);
    todo_stream_sort(list);
}

static void todo_prioritySort(TodoList *list, int argc, char *argv[])
{
    todo_stream_priorityScoreSort(list, todo_get_args(argc, argv, 2));
}