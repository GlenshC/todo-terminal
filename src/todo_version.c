#include "todo_version.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

const char TODO_APP_VERSION[16] = "1.0b";
const size_t TODO_APP_VERSION_SIZE = 16;

const char *known_versions[] = 
{
    "1.0b"
};
const size_t known_versions_size = sizeof(known_versions)/sizeof(*known_versions);

void todo_show_version()
{
    printf("%s\n", TODO_APP_VERSION);
}

unsigned int todo_version_id(char *version)
{
    if (!isdigit(version[0]))
    {
        return 0;
    }
    for (size_t i = 0; i < known_versions_size; i++)
    {
        if (strcmp(version, known_versions[i]) == 0)
        {
            return i+1;
        }
    }
    return 0;
}