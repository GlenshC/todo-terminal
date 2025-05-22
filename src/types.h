#ifndef TODO_TYPES_H
#define TODO_TYPES_H

#include <stdint.h>

#define SECONDS_IN_DAY (86400)
#define SECONDS_IN_HOUR (3600)
#define SECONDS_IN_MIN (60)

#define TODO_PRIORITY_BITS 2
#define TODO_DONE_BITS 1

typedef enum TodoListByteFields
{
    TODO_FIELD_DONE = 1,
    TODO_FIELD_PRIORITY = 2
} TodoListByteFields;


typedef long long pScore;

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
    char *title;
    char *desc;
    long long created;
    long long deadline;
    
    uint8_t titleSize;
    uint8_t descSize;
    uint8_t priority;
    uint8_t done;
} TodoT; // 56bytes - nopads

struct DynamicTodoList;

typedef int (*todotreeCmpFun)(struct DynamicTodoList *list, unsigned int a, unsigned int b);
typedef void (*todotreeDispFun)(struct DynamicTodoList *list, unsigned int value);

typedef struct DynamicTodoList
{
    
    /* Writeable */
    size_t size; 
    
    uint32_t *created; // STORED IN MINUTES
    uint32_t *deadline; // STORED IN DAYS
    
    uint8_t *titleSize;
    char **title; //editable
    
    uint8_t *descSize;
    char **desc; // editable

    uint8_t *priority; // 2 bits
    uint8_t *done; // 1 bit
    
    
    /* Runtime Data */
    size_t capacity;
    pScore *priorityScore;

    TRoot *sortedList;
    todotreeCmpFun sortingFunc;
    long long timeToday;
    uint8_t isAccending; 
    unsigned int energy; 
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

/* COMMANDS */
typedef enum TodoCommandsEnum {
    COMMAND_ADD,
    COMMAND_CLEAR,
    COMMAND_EDIT,
    COMMAND_GET,
    COMMAND_HELP,
    COMMAND_LIST,
    COMMAND_RANDOM,
    COMMAND_REMOVE,
    COMMAND_VERSION,
    COMMAND_VIEW,
} TodoCommandsEnum;

#endif