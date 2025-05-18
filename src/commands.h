#ifndef TODO_COMMANDS_INTERFACE_H
#define TODO_COMMANDS_INTERFACE_H

typedef enum TodoCommandsEnum {
    COMMAND_ADD,
    COMMAND_CLEAR,
    COMMAND_EDIT,
    COMMAND_GET,
    COMMAND_HELP,
    COMMAND_LIST,
    COMMAND_RANDOM,
    COMMAND_REMOVE,
    COMMAND_VERSION,
    COMMAND_VIEW,
} TodoCommandsEnum;

void todo_help(int argc, char *argv[]);
void todo_help_all(void);
void todo_help_command(TodoCommandsEnum cmdEnum);
void todo_help_error(const char *cmdStr);

#endif