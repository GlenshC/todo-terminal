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
        todo_GUI();//0   1    2    3    4    5    6    7    8    9
        int colors[] = {250, 246, 240, 229, 226, 220, 214, 208, 202, 196};
        const char *labels[] = {
            "Light Gray", "Silver", "Dark Gray", "Yellow (light)",
            "Yellow", "Warm Yellow", "Yellow-Orange", "Orange",
            "Red-Orange", "Red"
        };

        for (int i = 0; i < 10; i++) {
            printf("\x1b[38;5;%dm%s\n", colors[i], labels[i]);
        }

        printf("\x1b[0m"); // Reset
    }
}