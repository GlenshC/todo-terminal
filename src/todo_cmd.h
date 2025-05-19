
#ifndef TODO_CMD_INTERFACE_H
#define TODO_CMD_INTERFACE_H
#include "types.h"
// #include "gc_string.h"
typedef long long time_t;

#define targ_get(index, default) (((index) < argc) ? (argv[(index)]) : (default))
#define todo_cmd(cmd) strcmp(argv[1], cmd)
#define todo_cmdi(cmd, index) strcmp(argv[(index)], cmd)
#define isbounded(val, start, end)   (start <= val && val <= end)
#define isboundedx(val, start, end)   (start < val && val < end)

void todo_add(TodoList *list, char *title, char *description, TodoDate *tododate, unsigned int priority);
void todo_list(TodoList *list);
void todo_edit(TodoList *list, unsigned int index);
void todo_sorting_arg(TodoList *list, int argc, char *argv[]);

void todo_compute_priorityScores(TodoList *list);
pScore todo_get_priorityScore(TodoList *list, unsigned int index, pScore *scoreTable, time_t timeToday);
pScore *todo_get_todouserenergy(unsigned int energy);
time_t todo_get_timeToday(void);

void todo_cmd_get(TodoList *list);
void todo_cmd_random(TodoList *list);

#endif // #define TODO_CMD_INTERFACE_H
