
#ifndef TODO_VERSION_H
#define TODO_VERSION_H
#include "types.h"

extern const char TODO_APP_VERSION[16];
extern const size_t TODO_APP_VERSION_SIZE;

void todo_show_version();
unsigned int todo_version_id(char *version);

#endif // TODO_VERSION_H