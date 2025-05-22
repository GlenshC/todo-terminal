#include "todo_get_alg.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h> 
#include "stream.h"
#include "todo_error.h"
#include "todoio.h"

#define MIN(a, b) ((a < b) ? (a) : (b))
#define MAX(a, b) ((a > b) ? (a) : (b))
#define CLAMP(val, min, max) (((val) < (min)) ? (min) : (((val) > (max)) ? (max) : (val))) 


static pScore todo_get_todopressure(time_t deadline, time_t today);
static pScore todo_get_todoage(time_t created, time_t today);

static pScore TODO_ENERGY_WEIGHTS[] = {
    //critical, growth, distraction, waste
    60,  30, 70, 30, // Low Energy
    80,  70, 50, 10, // Med Energy
    95,  90, 30,  0  // High Energy
};

/* ===========================================
    PRIORITY SCORE
   ===========================================*/

   
uint32_t xorshift32(uint32_t *state) 
{
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return *state = x;
}


void todo_compute_priorityScores(TodoList *list)
{
    pScore *scoreTable = todo_get_todouserenergy(0);
    time_t timeToday = time(NULL);
    for (size_t i = 0; i < list->size; i++)
    {
        list->priorityScore[i] = todo_get_priorityScore(list, i, scoreTable, timeToday);
    }
}

pScore todo_get_priorityScore(TodoList *list, unsigned int index, pScore *scoreTable, time_t timeToday)
{

    pScore weigth = 0, pressure = 0, age = 0;

    
    weigth = scoreTable[todo_getBit(list->priority, index, TODO_PRIORITY_BITS)];
    
    if (list->deadline[index])
    {
        pressure = todo_get_todopressure(list->deadline[index], timeToday);
    }
    age = todo_get_todoage(list->created[index], timeToday);
    return weigth + pressure + age;
}

pScore *todo_get_todouserenergy(unsigned int *energyptr)
{
    char buffer[20];
    unsigned int energy = *energyptr;
    if (energy == 0)
    {
        printf("Enter energy levels? (1 - LOW, 2 - MEDIUM, 3 - HIGH): ");
        todo_getinput(buffer, 20);
        energy = strtoul(buffer, NULL, 0);
        if (energy == 0)
        {
            energy = 3;
        }
    }
    *energyptr = energy;

    switch(energy)
    {
        case 1: // low
        {
            return TODO_ENERGY_WEIGHTS;
        }
        case 2: // med
        {
            return TODO_ENERGY_WEIGHTS + 4;
        }
        default:
        {
            return TODO_ENERGY_WEIGHTS + 8;
        }
    }
}

/* ===========================================
    HELPER FUNCTIONS
   ===========================================*/
//



static pScore todo_get_todopressure(time_t deadline, time_t today)
{
    float penalty = 0, timeLeft, raw;

    timeLeft = (deadline - today) / (24.0f * 60.0f * 60.0f);
    raw = CLAMP(MAX_DAYS - timeLeft, 0, MAX_POINTS);
    
    if (timeLeft < 0)
    {
        penalty = timeLeft*-2;
    }
    return (long long)((raw / MAX_DAYS) * MAX_POINTS + penalty);
}


static pScore todo_get_todoage(time_t created, time_t today)
{
    time_t ageDay = (created - today) / (24*60*60);
    if (ageDay > 7)
        return 10;    
    else if (ageDay > 30)
        return 20;
    
    return 0;
}