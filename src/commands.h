#ifndef TODO_COMMANDS_INTERFACE_H
#define TODO_COMMANDS_INTERFACE_H

#include "types.h"
typedef struct {
    const char* short_flag;
    const char* long_flag;
    const char* description;
} Option;

typedef struct {
    const char* name;
    const char* description;
    const char* usage;
    const char* arguments;
    const char* examples;
    Option* options;
    int option_count;
} Command;

#endif