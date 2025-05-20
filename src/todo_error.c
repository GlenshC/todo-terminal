#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "todo_error.h"
#include "terminal_colors.h"

static const char *TODO_ERROR_MSGS[] = 
{
    TERMINAL_BOLD_RED "Error: No items found." TERMINAL_COLOR_RESET,
    TERMINAL_BOLD_RED "Error: Invalid ID." TERMINAL_COLOR_RESET,
    TERMINAL_BOLD_RED "Error: Invalid usage of command." TERMINAL_COLOR_RESET,
    TERMINAL_BOLD_RED "Error: Todo title cannot be empty." TERMINAL_COLOR_RESET,

};

static const size_t TODO_ERROR_MSGS_SIZE = sizeof(TODO_ERROR_MSGS)/sizeof(*TODO_ERROR_MSGS);

void todo__error(TodoErrors errCode)
{
    if (errCode < 0 && errCode >= TODO_ERROR_MSGS_SIZE)
    {
        printf("Error: invalid error code: %d\n", errCode);
        return;
    }
    
    printf("%s\n", TODO_ERROR_MSGS[errCode]);
}

void todo__debugerror(TodoErrors errCode, const char *file, int line)
{
    if (errCode < 0 && errCode >= TODO_ERROR_MSGS_SIZE)
    {
        printf("%s:%d: Error: todo_error invalid error code: %d\n", file, line, errCode);
        return;
    }
    
    printf("%s:%d: %s\n", file, line, TODO_ERROR_MSGS[errCode]);
}