
#ifndef TODO_TREE_H
#define TODO_TREE_H
#include "types.h"

TRoot *todo_tree_init(int a);
void todo_tree_free(void);
void todo_tree_push(void);
void todo_tree_remove(void);
void todo_tree_display(void);

unsigned int todo_tree_at(void);
// unsigned int todo_tree_getvalat(TodoList *list, TRoot *root, unsigned int value, todotreeCmpFun compare);

#endif // TODO_TREE_H