#include <stdio.h>
#include "todo.h"

int main(int argc, char *argv[])
{
    /* 
        first add terminal functionality
        todo add        //add a todo 
        todo list       //list all available
        togo random     //get a random task
        todo remove     //show list and prompt where to remove
        todo edit       //show list and prompt where to edit and prompts editing
    */
    if (argc > 1)
    {
        todo_CLI(argc, argv);
    }
    else 
    {
        todo_GUI();
    }
}