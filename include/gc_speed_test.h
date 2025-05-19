#ifndef GC_PERFORMANCE_TEST
#define GC_PERFORMANCE_TEST

#define TERMINAL_COLOR_RESET        "\x1b[0m"
#define TERMINAL_COLOR_BLUE         "\x1b[34m"

#include <stdio.h>

#ifdef _WIN32
    #define GC_WIN_SPEED_TEST
    #include <windows.h>
#endif // _WIN32

#ifdef GC_WIN_SPEED_TEST
    typedef struct GC_SpeedTest{
        char *name;
        LARGE_INTEGER stats[3]; // 0 freq 1 start 2 end
    } GC_SpeedTest;

    void GC_PTestStart(GC_SpeedTest *test, char *name)
    {
        test->name = name;
        LARGE_INTEGER *freq = &test->stats[0];
        LARGE_INTEGER *start = &test->stats[1];

        QueryPerformanceFrequency(freq);
        QueryPerformanceCounter(start);
    }

    void GC_PTestEnd(GC_SpeedTest *test)
    {
        LARGE_INTEGER *freq = &test->stats[0];
        LARGE_INTEGER *start = &test->stats[1];
        LARGE_INTEGER *end = &test->stats[2];

        QueryPerformanceCounter(end);
        double elapsed = (double)(end->QuadPart - start->QuadPart) / freq->QuadPart;
        printf(TERMINAL_COLOR_BLUE "\n%s - elapsed time: %lf seconds\n" TERMINAL_COLOR_RESET, test->name, elapsed);
    }
#endif //GC_WIN_SPEED_TEST

#define GC_PERFORMANCE_ITERATIONS(count) for (volatile long i = 0; i < count; i++)



#endif //GC_PERFORMANCE_TEST