
#include <stdio.h>
#include "gc_string.h"
#include "terminal_colors.h"
#include "todo_help.h"

#include "commands.h"

unsigned int TODO_COMMANDS_SIZE = sizeof(TODO_COMMANDS) / sizeof(*TODO_COMMANDS);

void todo_help_missing()
{
    printf(TERMINAL_COLOR_RED "Missing command. Type `todo help` for available commands.\n" TERMINAL_COLOR_RESET);
}

void todo_help_error(const char *cmdStr)
{
        printf(TERMINAL_COLOR_RED "Unknown command: '%s'\n", cmdStr);
        printf(TERMINAL_COLOR_YELLOW "Type "TERMINAL_COLOR_BOLD "`todo help`" TERMINAL_COLOR_RESET TERMINAL_COLOR_YELLOW " to see available commands.\n" TERMINAL_COLOR_RESET); 
}

void todo_help(int argc, char *argv[])
{
    // 1    2     3
    // todo help [command]
    if (argc == 3)
    {
        int notfound = 1;
        for (unsigned int i = 0; i < TODO_COMMANDS_SIZE; i++)
        {
            if (gc_str_strcmp(argv[2], TODO_COMMANDS[i].name) == 0)
            {
                todo_help_command(i);
                notfound = 0;
                break;
            }
        }
        if (notfound)
        {
            todo_help_error(argv[2]);
        }
    }
    else
    {
        todo_help_all();
    }
}

void todo_help_all()
{
    printf(TERMINAL_COLOR_DIM "For more information on a specific command, type:\n" TERMINAL_COLOR_RESET);
    printf(TERMINAL_COLOR_ITALIC"  todo help [COMMAND]\n" TERMINAL_COLOR_RESET);

    printf(TERMINAL_BOLD_YELLOW "\nUsage:\n" TERMINAL_COLOR_RESET);
    printf(TERMINAL_COLOR_ITALIC"  todo [COMMAND] [ARGS]\n" TERMINAL_COLOR_RESET);

    printf(TERMINAL_BOLD_YELLOW "\nCommands:\n" TERMINAL_COLOR_RESET);
    for (unsigned int i = 0; i < TODO_COMMANDS_SIZE; i++)
    {
        printf(TERMINAL_COLOR_CYAN"  %-10s " TERMINAL_COLOR_RESET TERMINAL_COLOR_DIM "%s\n" TERMINAL_COLOR_RESET, TODO_COMMANDS[i].name, TODO_COMMANDS[i].description);
    }
}

void todo_help_command(TodoCommandsEnum cmdEnum)
{
    TodoCommand *cmd = (TodoCommand *)&(TODO_COMMANDS[cmdEnum]);
    unsigned int argsize = cmd->argumentsSize;
    unsigned int optionsize = cmd->optionsSize;
    unsigned int examplesize = cmd->examplesSize;

    printf(
        TERMINAL_BOLD_YELLOW "Usage:\n" TERMINAL_COLOR_RESET
        TERMINAL_COLOR_ITALIC "  %s\n\n" TERMINAL_COLOR_RESET
        "%s\n",  
        cmd->usage,
        cmd->description
    );

    if (cmd->long_description)
    {
        printf(
            "\n%s\n",
            cmd->long_description
        );
    }

    if (argsize) {
        printf(TERMINAL_BOLD_YELLOW "\nArguments:\n"TERMINAL_COLOR_RESET);
        for (unsigned int i = 0; i < argsize; i++)
        {
            printf(
                TERMINAL_COLOR_CYAN "  %-30s " TERMINAL_COLOR_RESET
                TERMINAL_COLOR_DIM "%s\n" TERMINAL_COLOR_RESET, 
                cmd->arguments[i].name,
                cmd->arguments[i].description
            );
        }
    }
    
    if (optionsize) {
        printf(TERMINAL_BOLD_YELLOW "\nOptions:\n" TERMINAL_COLOR_RESET);
        for (unsigned int i = 0; i < optionsize; i++)
        {
            printf(
                TERMINAL_COLOR_CYAN "  %-4s %-25s " TERMINAL_COLOR_RESET
                TERMINAL_COLOR_DIM "%s\n" TERMINAL_COLOR_RESET, 
                cmd->options[i].short_flag,
                cmd->options[i].long_flag,
                cmd->options[i].description
            );
        }
    }

    if (examplesize)
    {
        printf(TERMINAL_BOLD_YELLOW "\nExamples:\n" TERMINAL_COLOR_RESET);
        for (unsigned int i = 0; i < examplesize; i++)
        {
            printf(
                TERMINAL_COLOR_ITALIC "  %s\n" TERMINAL_COLOR_RESET, 
                cmd->examples[i]
            );
        }
    }
    /* 
    usage
    description
    long description e
    arguments r/o e
    options e
    examples e
    */
}