#ifndef TODO_GET_ALGORITHM_H
#define TODO_GET_ALGORITHM_H

#include "types.h"

#define MAX_DAYS (10)
#define MAX_HOURS (24*MAX_DAYS)
#define MAX_POINTS (50)

void todo_compute_priorityScores(TodoList *list);
pScore todo_get_priorityScore(TodoList *list, unsigned int index, pScore *scoreTable, time_t timeToday);

pScore *todo_get_todouserenergy(unsigned int *energyptr); 
uint32_t xorshift32(uint32_t *state);

#endif //TODO_GET_ALGORITHM_H