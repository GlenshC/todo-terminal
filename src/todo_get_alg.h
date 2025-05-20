#ifndef TODO_GET_ALGORITHM_H
#define TODO_GET_ALGORITHM_H

#include "types.h"

void todo_compute_priorityScores(TodoList *list);
pScore todo_get_priorityScore(TodoList *list, unsigned int index, pScore *scoreTable, time_t timeToday);

pScore *todo_get_todouserenergy(unsigned int *energyptr); 
uint32_t xorshift32(uint32_t *state);

#endif //TODO_GET_ALGORITHM_H