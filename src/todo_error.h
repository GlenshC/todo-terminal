
#ifndef GC_TODO_ERROR_H
#define GC_TODO_ERROR_H

typedef enum TodoErrors{
    TODO_ERROR_EMPTY,
    TODO_ERROR_INVALID_ID,
    TODO_ERROR_INVALID_CMD_USAGE,
    TODO_ERROR_ADD_TITLEARG_EMPTY,

} TodoErrors;


void todo__error(TodoErrors errCode);
void todo__debugerror(TodoErrors errCode, const char *file, int line);

#ifdef GC_DEBUG
#define  todo_error(errCode) todo__debugerror(errCode, __FILE__, __LINE__)
#else
#define  todo_error(errCode) todo__error(errCode)
#endif // GC_DEBUG


#endif // GC_TODO_ERROR_H
