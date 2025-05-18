
#ifndef TODO_TREE_COMPARE_FUNCTIONS_H
#define TODO_TREE_COMPARE_FUNCTIONS_H
#include "types.h"

int todo_tree_priorityCompare(TodoList *list, unsigned int a, unsigned int b);
int todo_tree_defaultCompare(TodoList *list, unsigned int a, unsigned int b);
int todo_tree_deadlineCompare(TodoList *list, unsigned int a, unsigned int b);
int todo_tree_createdCompare(TodoList *list, unsigned int a, unsigned int b);
int todo_tree_titleCompare(TodoList *list, unsigned int a, unsigned int b);
int todo_tree_priorityScoreCompare(TodoList *list, unsigned int a, unsigned int b);

#endif // TODO_TREE_COMPARE_FUNCTIONS_H