#ifndef GC_PERFORMANCE_TEST
#define GC_PERFORMANCE_TEST

typedef struct GC_SpeedTest* GC_SpeedTest;

#define PerfomanceTime_Start(test, name)
#define PerfomanceTime_End(test)
#define PerfomanceTime_Total()
#define PerfomanceTime_Reset()
#define mk_speedtest(...)

#ifdef GC_DEBUG
#ifdef _WIN32
#define GC_WIN_SPEED_TEST
#endif // _WIN32

#ifdef GC_WIN_SPEED_TEST
    #undef mk_speedtest
    #undef PerfomanceTime_Start
    #undef PerfomanceTime_End
    #undef PerfomanceTime_Total
    #undef PerfomanceTime_Reset
    
    void GC_PTestStart(GC_SpeedTest *test, char *name);
    void GC_PTestEnd(GC_SpeedTest test);
    double GC_PTestTotal(void);
    void GC_PTestReset(void);
    
    #define mk_speedtest(...) GC_SpeedTest __VA_ARGS__
    #define PerfomanceTime_Start(test, name) GC_PTestStart(test, name)
    #define PerfomanceTime_End(test) GC_PTestEnd(test)
    #define PerfomanceTime_Total() GC_PTestTotal()
    #define PerfomanceTime_Reset() GC_PTestReset()

#endif //GC_WIN_SPEED_TEST

#define GC_PERFORMANCE_ITERATIONS(count) for (volatile long i = 0; i < count; i++)

#endif //GC_DEBUG 
#endif //GC_PERFORMANCE_TEST