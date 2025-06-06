
#ifndef GC_TODO_STREAM_H
#define GC_TODO_STREAM_H

#include <stdio.h>
#include "types.h"
#include "bitwise.h"

#define TODO_STREAM_MAX_TODOS 1024

int todo_stream_write(TodoList *list);
int todo_stream_read(TodoList *todolist);

void todo_stream_free_todolist(TodoList *todolist);
int todo_stream_init(TodoList *list, size_t capacity);
int todo_stream_push(TodoList *list, TodoT *todo);
int todo_stream_remove(TodoList *list, size_t index);
void todo_stream_sort(TodoList *list);
void todo_stream_priorityScoreSort(TodoList *list, unsigned int energy);

#endif // GC_TODO_STREAM_H included