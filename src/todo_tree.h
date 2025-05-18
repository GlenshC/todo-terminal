
#ifndef TODO_TREE_H
#define TODO_TREE_H
#include "types.h"

TRoot *todo_tree_init();
void todo_tree_free(TRoot **root);
void todo_tree_push(TodoList *list, TRoot *root, unsigned int value, todotreeCmpFun compare);
void todo_tree_remove(TodoList *list, TRoot *root, unsigned int value, todotreeCmpFun compare);
void todo_tree_display(TodoList *list, TRoot *root, todotreeDispFun display);

unsigned int todo_tree_at(TodoList *list, TRoot *root, unsigned int index);
// unsigned int todo_tree_getvalat(TodoList *list, TRoot *root, unsigned int value, todotreeCmpFun compare);

#endif // TODO_TREE_H