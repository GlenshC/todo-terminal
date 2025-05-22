#ifndef STREAM_WRITER_H
#define STREAM_WRITER_H

#include <stdio.h>
#include "types.h"

static const char TODO_FILE_PADDING_END[] =   {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC};

void stream_writer(TodoList *todolist, FILE *file);

#endif //STREAM_WRITER_H