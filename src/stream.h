
#ifndef GC_TODO_STREAM_H
#define GC_TODO_STREAM_H

#include <stdio.h>
#include "types.h"

// int todo_stream_isopen();

int todo_stream_write(TodoT *todo);
int todo_stream_read(TodoList *todolist);
void todo_stream_free_todolist(TodoList *todolist);

#endif // GC_TODO_STREAM_H included