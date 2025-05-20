

#include "gc_logs.h"

#include "gc_speed_test.h"
#include "todo.h"
#include "todo_api.h"
#include "todo_version.h"

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
    else if (todo_cmd("version") == 0)
    {
        todo_show_version();
        return;   
    }
// #undef GC_DEBUG
#ifdef GC_DEBUG
    double totalTime = 0;
    GC_SpeedTest readtest, writetest, functest, freetest;
#endif

    PerfomanceTime_Start(&readtest, "Read test");
    todo_readlist(&list);
    PerfomanceTime_End(&readtest, &totalTime);
    
    PerfomanceTime_Start(&functest, "Function test");
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
        else if (todo_cmd("view") == 0)
        {
            cmd_view(&list, argc, argv);   
        }
        else 
        {
            todo_help_error(argv[1]);            
        }
    }
    PerfomanceTime_End(&functest, &totalTime);
    
    PerfomanceTime_Start(&writetest, "Write Test");
    todo_writelist(&list);
    PerfomanceTime_End(&writetest, &totalTime);
    
    PerfomanceTime_Start(&freetest, "Free resource Test");
    todo_freelist(&list); // remove if you want faster exit
    PerfomanceTime_End(&freetest, &totalTime);

#ifdef GC_DEBUG
    printf("Total time elapsed: %lf\n", totalTime);
#endif

}

void todo_GUI()
{
    GC_LOG(TERMINAL_COLOR_BLUE "Coming soon: Todo-GUI\n" TERMINAL_COLOR_RESET);
}