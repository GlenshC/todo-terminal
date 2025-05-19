#ifndef TODO_ARGS_INTERFACE_H
#define TODO_ARGS_INTERFACE_H
#include "types.h"

void todo_cmd_add(TodoList *list, int argc, char *argv[]);
void todo_cmd_edit(TodoList *list);
void todo_cmd_clear(int argc, char *argv[]);
void todo_cmd_remove(TodoList *list);

void todo_sorting_arg(TodoList *list, int argc, char *argv[]);
unsigned int todo_get_args(int argc, char *argv[], unsigned int index);

#endif // TODO_ARGS_INTERFACE_H