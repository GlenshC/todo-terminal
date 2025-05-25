#include "gc_speed_test.h"
#define TERMINAL_COLOR_RESET        "\x1b[0m"
#define TERMINAL_COLOR_BLUE         "\x1b[34m"


#ifdef GC_DEBUG
#include <stdio.h>

#ifdef _WIN32
    #include <windows.h>
#endif // _WIN32

double speed_test_total = 0;
#ifdef GC_WIN_SPEED_TEST
    struct GC_SpeedTest{
        char *name;
        LARGE_INTEGER stats[3]; // 0 freq 1 start 2 end
    };

    void GC_PTestStart(GC_SpeedTest *testptr, char *name)
    {
        *testptr = malloc(sizeof(struct GC_SpeedTest));
        GC_SpeedTest test = *testptr;

        test->name = name;
        LARGE_INTEGER *freq = &test->stats[0];
        LARGE_INTEGER *start = &test->stats[1];

        QueryPerformanceFrequency(freq);
        QueryPerformanceCounter(start);
    }

    void GC_PTestEnd(GC_SpeedTest test)
    {
        LARGE_INTEGER *freq = &test->stats[0];
        LARGE_INTEGER *start = &test->stats[1];
        LARGE_INTEGER *end = &test->stats[2];

        QueryPerformanceCounter(end);
        double elapsed = (double)(end->QuadPart - start->QuadPart) / freq->QuadPart;
        printf(TERMINAL_COLOR_BLUE "%s - elapsed time: %lf seconds\n" TERMINAL_COLOR_RESET, test->name, elapsed);
        speed_test_total += elapsed;
    }

    double GC_PTestTotal(void)
    {
        return speed_test_total; 
    }
    
    void GC_PTestReset(void)
    {
        speed_test_total = 0;
    }


#endif //GC_WIN_SPEED_TEST
#endif //GC_DEBUG 