#include "commands.h"

#include <stdio.h>
#include "gc_string.h"
#include "terminal_colors.h"

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
    const char **examples;
    const TodoCmdArguments *arguments;
    const TodoCmdOptions *options;
    const unsigned int examplesSize;
    const unsigned int argumentsSize;
    const unsigned int optionsSize;
}TodoCommand;

static const TodoCommand TODO_COMMANDS[] = {
    {
        "add",
        "Adds a new todo item.",
        "Adds a new todo item with optional metadata.",
        "todo add <title> [OPTIONS]",
        (const char *[]){
            "todo add \"Read book\"",
            "todo add \"Finish project\" -d \"Due next week\" -D 25/05/2025 -p critical",
            "todo add \"Scroll Instagram\" -p 4"
        },
        (TodoCmdArguments[]){
            { "title", "Title of the todo item" }
        },
        (TodoCmdOptions[]){
            { "-d", "--description <text>", "Description of the todo item" },
            { "-D", "--deadline <DD/MM/YYYY>", "Deadline (must be a future date)" },
            { "-p", "--priority <level|1-4>", "Priority level:" },
            { "", "", "  1 - critical" },
            { "", "", "  2 - growth (default)" },
            { "", "", "  3 - distraction" },
            { "", "", "  4 - waste" },
        },
        3, 1, 7
    },
    {
        "clear",
        "Empties the todo list.",
        NULL,
        "todo clear [OPTIONS]",
        NULL,
        NULL,
        (TodoCmdOptions[]){
            { "", "--force", "Skips the confirmation prompt (\"Are you sure? [yes/no]\")" }
        },
        0, 0, 1
    },
    {
        "edit",
        "Edits a todo item.",
        "Edits a todo item. If an id is provided, edits that item directly; otherwise, shows the todo list for selection.",
        "todo edit [id]",
        NULL,
        (TodoCmdArguments[]){
            { "id", "(Optional) ID of the todo item to edit" }
        },
        NULL,
        0, 1, 0
    },
    {
        "get",
        "Gets the most important task.",
        "Selects the most important task to do next, based on deadlines, priority, and your energy level. Waste tasks may still be selected when energy is low, but only if they are the most appropriate option.",
        "todo get [energy]",
        NULL,
        (TodoCmdArguments[]){
            { "energy", "(optional) Your current energy level. If omitted, you will be prompted to enter it. Possible values:\n  1 - low\n  2 - medium\n  3 - high" }
        },
        NULL,
        0, 1, 0
    },
    {
        "help",
        "Shows help information.",
        "Shows general help or detailed help for a specific command.",
        "todo help [command]",
        (const char *[]){
            "todo help",
            "todo help add"
        },
        (TodoCmdArguments[]){
            { "command", "(optional) The command to get help for. If omitted, displays this general help message." }
        },
        NULL,
        2, 1, 0
    },
    {
        "list",
        "Shows all todos.",
        "Shows all todos and exits.",
        "todo list [OPTIONS]",
        NULL,
        NULL,
        (TodoCmdOptions[]){
            { "-t", "--title", "Sorts todos alphabetically by title" },
            { "-D", "--deadline", "Sorts todos by deadline: overdue first, then soonest, then farthest" },
            { "-p", "--priority", "Sorts todos by priority: critical, growth, distraction, waste" },
            { "-c", "--created", "Sorts todos by creation time (oldest first)" },
            { "-b", "--best", "Sorts todos by recommended action priority" }
        },
        0, 0, 5
    },
    {
        "random",
        "Selects a task randomly.",
        "Selects a task to do with some randomness. Critical, growth, and distraction tasks are favored. Waste tasks are usually ignored, but may be chosen when energy is low — even if more important tasks exist.",
        "todo random [energy]",
        NULL,
        (TodoCmdArguments[]){
            { "energy", "(optional) Your current energy level. If omitted, you will be prompted to enter it. Possible values:\n  1 - low\n  2 - medium\n  3 - high" }
        },
        NULL,
        0, 1, 0
    },
    {
        "rm",
        "Removes a todo item.",
        "Removes a todo item.\n\nIf no ID is provided, the todo list will be shown and you will be prompted to select an item to remove. If an ID is provided, the item will be deleted immediately.",
        "todo rm [id] [OPTIONS]",
        NULL,
        (TodoCmdArguments[]){
            { "id", "(optional) ID of the todo item to remove" }
        },
        (TodoCmdOptions[]){
            { "-t", "--title", "Sort todos alphabetically by title" },
            { "-D", "--deadline", "Sort todos by deadline: overdue → soonest → farthest" },
            { "-p", "--priority", "Sort todos by priority: critical, growth, distraction, waste" },
            { "-c", "--created", "Sort todos by creation time (oldest first)" },
            { "-b", "--best", "Sort todos by recommended action priority" }
        },
        0, 1, 5
    },
    {
        "version",
        "Displays the current version.",
        NULL,
        "todo version",
        NULL,
        NULL,
        NULL,
        0, 0, 0
    },
    {
        "view",
        "Views a specific todo item.",
        "Displays detailed information about a specific todo item.\n\nIf no ID is provided, the todo list will be shown and you will be prompted to select an item to view.",
        "todo view [id]",
        NULL,
        (TodoCmdArguments[]){
            { "id", "(optional) ID of the todo item to view" }
        },
        NULL,
        0, 1, 0
    }
};

unsigned int TODO_COMMANDS_SIZE = sizeof(TODO_COMMANDS) / sizeof(*TODO_COMMANDS);

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