#ifndef TODO_TYPES_H
#define TODO_TYPES_H

#include <time.h>

typedef struct TodoDate
{
    int day,
        month,
        year;
} TodoDate;

typedef struct TodoT
{
    size_t titleSize;
    char *title;
    
    size_t descSize;
    char *desc;

    size_t priority;
    time_t created;
    time_t deadline;
} TodoT; // 56bytes - nopads



typedef struct DynamicTodoList
{
    size_t size;
    size_t capacity;

    size_t *titleSize;
    char **title;
    
    size_t *descSize;
    char **desc;

    size_t *priority;
    time_t *created;
    time_t *deadline;
} TodoList;
#endif