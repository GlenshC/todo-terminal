
#ifndef GC_TODO_FILE_H
#define GC_TODO_FILE_H

#include <stdio.h>

FILE *todo_file();
int todo_file_isopen();
void todo_file_close();

#define todo_fprintf(file, format, ...) (todo_file_isopen()) ? fprintf(file, format, ##__VA_ARGS__) : 0

#endif // GC_TODO_FILE_H included