#ifndef TODO_ARGS_INTERFACE_H
#define TODO_ARGS_INTERFACE_H
#include "types.h"

void todo_cmd_add(TodoList *list, int argc, char *argv[]);
void todo_cmd_edit(TodoList *list, int argc, char *argv[]);
void todo_cmd_view(TodoList *list, int argc, char *argv[]);
void todo_cmd_clear(int argc, char *argv[]);
void todo_cmd_remove(TodoList *list, int argc, char *argv[]);
void todo_cmd_done(TodoList *list, int argc, char *argv[]);
void todo_cmd_undo(TodoList *list, int argc, char *argv[]);


void todo_sorting_arg(TodoList *list, int argc, char *argv[]);
unsigned int todo_get_args(int argc, char *argv[], unsigned int index);
unsigned int todo_arg_id(int argc, char *argv[], unsigned int index);
uint8_t todo_priority_input(char *str, uint8_t defaultpriority);

#endif // TODO_ARGS_INTERFACE_H