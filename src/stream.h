
#ifndef GC_TODO_STREAM_H
#define GC_TODO_STREAM_H

#include <stdio.h>
#include "types.h"


int todo_stream_write(TodoList *list);
int todo_stream_read(TodoList *todolist);

void todo_stream_free_todolist(TodoList *todolist);
int todo_stream_init(TodoList *list, size_t capacity);
int todo_stream_grow(TodoList *list, size_t newCapacity);
int todo_stream_push(TodoList *list, TodoT *todo);
int todo_stream_remove(TodoList *list, size_t index);

#endif // GC_TODO_STREAM_H included