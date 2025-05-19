

#include "gc_logs.h"

#include "gc_speed_test.h"
#include "todo.h"
#include "todo_api.h"

#ifdef _WIN32
#include <windows.h>
#endif

void todo_CLI(int argc, char *argv[])
{
    TodoList list = {};

    if (argc < 2)
    {
        todo_help_missing();
    }

    if (todo_cmd("clear") == 0)
    {
        cmd_clear(argc, argv);
    }
    else if (todo_cmd("help") == 0)
    {
        todo_help(argc, argv); 
        return;
    }

    todo_readlist(&list);
    
    {
        if (todo_cmd("add") == 0)
        {
            cmd_add(&list, argc, argv);
        }
        else if (todo_cmd("rm") == 0)
        {
            cmd_remove(&list, argc, argv);
            
        }
        else if (todo_cmd("list") == 0)
        {
            cmd_list(&list, argc, argv);
            
        }
        else if (todo_cmd("get") == 0)
        {
            cmd_get(&list, argc, argv);
            
        }
        else if (todo_cmd("random") == 0)
        {            
            cmd_random(&list, argc, argv);
            
        }
        else if (todo_cmd("edit") == 0)
        {
            cmd_edit(&list, argc, argv);
            
        }
        else 
        {
            todo_help_error(argv[1]);            
        }
    }

    todo_writelist(&list);
#ifdef GC_DEBUG
    todo_freelist(&list); // remove if you want faster exit
#endif

}

void todo_GUI()
{
    GC_LOG("GUI");
}