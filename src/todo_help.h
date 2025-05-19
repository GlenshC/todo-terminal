#ifndef TODO_HELP_COMMANDS_INTERFACE_H
#define TODO_HELP_COMMANDS_INTERFACE_H

#include "types.h"
typedef enum TodoCommandsEnum TodoCommandsEnum;

void todo_help(int argc, char *argv[]);
void todo_help_all(void);
void todo_help_command(TodoCommandsEnum cmdEnum);
void todo_help_error(const char *cmdStr);
void todo_help_missing();

#endif