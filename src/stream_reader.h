#ifndef STREAM_READER_H
#define STREAM_READER_H

#include <stdio.h>
#include "types.h"

#define STREAM_STARTING_CAPCITY 16
#define STREAM_CAPACITY_MARGIN 4
#define gc_binceil64(x) (--(x), (x) |= (x) >> 1, (x) |= (x) >> 2, (x) |= (x) >> 4, (x) |= (x) >> 8, (x) |= (x) >> 16, (x) |= (x) >> 32, ++(x))

int todo_stream_init(TodoList *list, size_t capacity);
void stream_reader(TodoList *todolist, FILE *file);

#endif