

#include "gc_logs.h"

#include "gc_speed_test.h"
#include "todo.h"
#include "todo_api.h"
#include "todo_version.h"
#include "terminal_colors.h"
#include "todo_hash.h"

#ifdef _WIN32
#include <windows.h>
#endif

void todo_CLI(int argc, char *argv[])
{
    // mk_speedtest(TODOAPPTEST);
    
    TodoList list = {};
    
    if (argc < 2)
    {
        todo_help_missing();
    }
    todo_readlist(&list);
    
    
    
    // PerfomanceTime_Start(&TODOAPPTEST, "Todo App");
    unsigned int cmd_hash = todo_hash_command(argv[1]);
    // GC_PERFORMANCE_ITERATIONS(1024)
    {
    switch (cmd_hash)
    {
        case TODO_HASH_VERSION_CMD:
        case TODO_HASH_CLEAR_CMD:
        case TODO_HASH_HELP_CMD: 
        {
            switch (cmd_hash)
            {
                case TODO_HASH_VERSION_CMD:
                {
                    if (todo_cmd("version") == 0)
                    {
                        todo_show_version();
                    }
                    else
                        todo_help_error(argv[1]);
                    break;
                }
                case TODO_HASH_CLEAR_CMD:
                {
                    if (todo_cmd("clear") == 0)
                    {
                        cmd_clear(argc, argv);
                    }
                    else
                        todo_help_error(argv[1]);
                    break;
                }
                case TODO_HASH_HELP_CMD:
                {
                    if (todo_cmd("help") == 0)
                    {
                        todo_help(argc, argv); 
                    }
                    else
                        todo_help_error(argv[1]);
                    break;
                }
                default:
                {
                    todo_help_error(argv[1]);
                    break;
                }
            }
            break;
        }
        case TODO_HASH_ADD_CMD: 
        case TODO_HASH_DONE_CMD:
        case TODO_HASH_EDIT_CMD:
        case TODO_HASH_GET_CMD: 
        case TODO_HASH_LIST_CMD:
        case TODO_HASH_RANDOM_CMD:
        case TODO_HASH_RM_CMD:  
        case TODO_HASH_UNDO_CMD:
        case TODO_HASH_VIEW_CMD:
        {
            switch (cmd_hash)
            {
                case TODO_HASH_ADD_CMD:
                {
                    if (todo_cmd("add") == 0)
                    {
                        cmd_add(&list, argc, argv);
                    }
                    else
                        todo_help_error(argv[1]);
                    break;
                }
                case TODO_HASH_DONE_CMD:
                {
                    if (todo_cmd("done") == 0)
                    {
                        cmd_done(&list, argc, argv);
                    }
                    else
                        todo_help_error(argv[1]);
                    break;
                }
                case TODO_HASH_EDIT_CMD:
                {
                    if (todo_cmd("edit") == 0)
                    {
                        cmd_edit(&list, argc, argv);
                    }
                    else
                        todo_help_error(argv[1]);
                    break;
                }
                case TODO_HASH_GET_CMD:
                {
                    if (todo_cmd("get") == 0)
                    {
                        cmd_get(&list, argc, argv);
                    }
                    else
                        todo_help_error(argv[1]);
                    break;
                }
                case TODO_HASH_LIST_CMD:
                {
                    if (todo_cmd("list") == 0)
                    {
                        cmd_list(&list, argc, argv);
                    }
                    else
                        todo_help_error(argv[1]);
                    break;
                }
                case TODO_HASH_RANDOM_CMD:
                {
                    if (todo_cmd("random") == 0)
                    {            
                        cmd_random(&list, argc, argv);
                    }
                    else
                        todo_help_error(argv[1]);
                    break;
                    }
                case TODO_HASH_RM_CMD:
                {
                    if (todo_cmd("rm") == 0)
                    {
                        cmd_remove(&list, argc, argv);
                    }
                    else
                        todo_help_error(argv[1]);
                    break;
                }
                case TODO_HASH_UNDO_CMD:
                {
                    if (todo_cmd("undo") == 0)
                    {
                        cmd_undo(&list, argc, argv);   
                    }
                    else
                        todo_help_error(argv[1]);
                    break;
                }
                case TODO_HASH_VIEW_CMD:
                {
                    if (todo_cmd("view") == 0)
                    {
                        cmd_view(&list, argc, argv);   
                    }
                    else
                        todo_help_error(argv[1]);
                    break;
                }
                default:
                {
                    todo_help_error(argv[1]);
                    break;
                }
            }
            break;
        }
    }
    }
    
    todo_writelist(&list);
    todo_freelist(&list);

#ifdef GC_DEBUG
    printf("Total time elapsed: %lf\n", PerfomanceTime_Total());
#endif

}

void todo_GUI()
{
    GC_LOG(TERMINAL_COLOR_BLUE "Coming soon: Todo-GUI\n" TERMINAL_COLOR_RESET);
}