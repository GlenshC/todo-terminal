#include "commands.h"

#include <stdio.h>

typedef struct TodoCmdArguments {
    const char *name;
    const char *description;
} TodoCmdArguments;

typedef struct TodoCmdOptions {
    const char *short_flag;
    const char *long_flag;
    const char *description;
} TodoCmdOptions;

typedef struct 
{
    const char *name;
    const char *description;
    const char *long_description;
    const char *usage;
    const TodoCmdArguments *arguments;
    const TodoCmdOptions *options;
    unsigned int argumentsSize;
    unsigned int optionsSize;
}TodoCommand;


TodoCommand TODO_COMMANDS[] = {
    {
        "add", "Adds a new todo item with optional metadata.", NULL,
        "todo add <title> [OPTIONS]", 
        {
            {"title", "Title of the todo item"}
        },
        {
            {"title", "Title of the todo item"}
        },

    }
};
unsigned int TODO_COMMANDS_SIZE = sizeof(TODO_COMMANDS) / sizeof(*TODO_COMMANDS);

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

void todo_help_all()
{
    printf("For more information on a specific command, type:\n");
    printf("  todo help [COMMAND]\n");

    printf("\nUsage:\n");
    printf("  todo [COMMAND] [ARGS]\n");

    printf("\nCommands:\n");
    for (unsigned int i = 0; i < TODO_COMMANDS_SIZE; i++)
    {
        printf("  %-10s %s\n", TODO_COMMANDS[i].name, TODO_COMMANDS[i].description);
    }
}

void todo_help_command(TodoCommandsEnum cmdEnum)
{
    printf(
        "Usage:\n"
        "%s\n\n"
        "%s\n\n", 
        TODO_COMMANDS[cmdEnum].usage,
        TODO_COMMANDS[cmdEnum].description
    );

    if (TODO_COMMANDS[cmdEnum].long_description)
    {
        printf(
            "%s\n\n",
            TODO_COMMANDS[cmdEnum].long_description
        );
    }

    if ()

    printf("%s")
    /* 
    usage
    description
    long description e
    arguments r/o e
    options e
    examples e
    */
}