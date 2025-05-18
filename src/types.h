#ifndef TODO_TYPES_H
#define TODO_TYPES_H

#include <stdint.h>

typedef uint8_t pScore;

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
    long long created;
    long long deadline;
} TodoT; // 56bytes - nopads

struct DynamicTodoList;

typedef int (*todotreeCmpFun)(struct DynamicTodoList *list, unsigned int a, unsigned int b);
typedef void (*todotreeDispFun)(struct DynamicTodoList *list, unsigned int value);

typedef struct DynamicTodoList
{
    
    /* Writeable */
    size_t size; 
    
    size_t *titleSize;
    char **title; //editable
    
    size_t *descSize;
    char **desc; // editable
    long long *deadline; // editable
    long long *created;
    uint8_t *priority;
    
    
    /* Runtime Data */
    size_t capacity;
    pScore *priorityScore;

    TRoot *sortedList;
    todotreeCmpFun sortingFunc;
    uint8_t isAccending; 
} TodoList;

/*
    Priority
    0 = urgent important (Critical)
    1 = not urgent important (Growth)
    2 = urgent not important (Distraction)
    3 = not urgent not important (Waste)
    */


typedef enum {
    TODO_SORT_DEFAULT,
    TODO_SORT_PRIORITY,
    TODO_SORT_DEADLINE,
    TODO_SORT_CREATED,
    TODO_SORT_TITLE
} todo_TreeSortType;


#endif