#include "time.h"
#include <Windows.h>

// This entire file is untested.
void TS_Chrono_SmallSleep(unsigned long time, enum TS_Chrono_TimeUnit unit){
    unsigned long milliseconds = (unit==TS_Chrono_Millisecond)?time:(time/1000000);
    Sleep(milliseconds);
}

void TS_Chrono_Sleep(void *ctx, unsigned long time, enum TS_Chrono_TimeUnit unit){
    unsigned long start_time = TS_Chrono_GetTime(unit);
    TS_Chrono_GC(ctx);
    unsigned long diff = time-(TS_Chrono_GetTime(unit)-start_time);
    TS_Chrono_SmallSleep(diff, unit);
}

unsigned long long TS_Chrono_GetTime(enum TS_Chrono_TimeUnit unit){
    // Avoid the conditional on ever call using a tautological assignment.
    static LARGE_INTEGER start, frequency;
    if(!start.QuadPart){
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start);
    }

    LARGE_INTEGER end;
    QueryPerformanceCounter(&end);
    
    const double factor = ((unit==TS_Chrono_Nanosecond)?1000000000.0:1000.0) / (double)(frequency.QuadPart);
    
    return (end.QuadPart-start.QuadPart)*factor;
}
