
#ifndef GC_TODO_API_H
#define GC_TODO_API_H

    #include "types.h"
    #include "todo_help.h"

    #define todo_cmd(cmd) strcmp(argv[1], cmd)
    
    void cmd_clear(int argc, char *argv[]);

    void cmd_add(TodoList *list, int argc, char *argv[]);
    void cmd_remove(TodoList *list, int argc, char *argv[]);            
    void cmd_list(TodoList *list, int argc, char *argv[]);
    void cmd_get(TodoList *list, int argc, char *argv[]);
    void cmd_random(TodoList *list, int argc, char *argv[]);
    void cmd_edit(TodoList *list, int argc, char *argv[]);
    void cmd_view(TodoList *list, int argc, char *argv[]);
    
    void todo_readlist(TodoList *list);
    void todo_writelist(TodoList *list);
    void todo_freelist(TodoList *list);

#endif // GC_TODO_API_H