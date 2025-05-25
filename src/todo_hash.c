#include "todo_hash.h"

unsigned int todo_hash_command(char *str)
{
    return (((uint16_t *)str)[0]) % 40;
}