#ifndef TODO_TYPES_H
#define TODO_TYPES_H

#include <time.h>
#include <stdint.h>

typedef struct TNode{
    unsigned int value;
    uint16_t valid;
    int16_t balance; // (-) left heavy, (0) is balance, (+) right heavy
    struct TNode *left;
    struct TNode *right;
} TRoot, TNode;

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

struct DynamicTodoList;

typedef int (*todotreeCmpFun)(struct DynamicTodoList *list, unsigned int a, unsigned int b);
typedef void (*todotreeDispFun)(struct DynamicTodoList *list, unsigned int value);

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

    TRoot *sortedList;
    todotreeCmpFun sortingFunc;
} TodoList;




typedef enum {
    TODO_SORT_DEFAULT,
    TODO_SORT_PRIORITY,
    TODO_SORT_DEADLINE,
    TODO_SORT_CREATED,
    TODO_SORT_TITLE
} todo_TreeSortType;


#endif