
#ifndef TODO_CMD_INTERFACE_H
#define TODO_CMD_INTERFACE_H
#include "types.h"

#define targ_get(index, default) (((index) < argc) ? (argv[(index)]) : (default))
#define todo_cmd(cmd) strcmp(argv[1], cmd)
#define todo_cmdi(cmd, index) strcmp(argv[(index)], cmd)
#define isbounded(val, start, end)   (start <= val && val <= end)
#define isboundedx(val, start, end)   (start < val && val < end)

void todo_add(TodoList *list, char *title, char *description, TodoDate *tododate, unsigned int priority);
void todo_list(TodoList *list);
void todo_edit(TodoList *list, unsigned int index);
void todo_sorting_arg(TodoList *list, int argc, char *argv[]);

#endif // #define TODO_CMD_INTERFACE_H
