#include "sm_wrapper.h"
#include "time.h"
#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>
#include <stdio.h>

static mach_timebase_info_data_t timebase;

void TS_Chrono_SmallSleep(unsigned long time, enum TS_Chrono_TimeUnit unit){
    const unsigned long micro_time = (unit==TS_Chrono_Millisecond)?(time*1000):(time/1000);
    usleep(micro_time);
}

void TS_Chrono_Sleep(void *ctx, unsigned long time, enum TS_Chrono_TimeUnit unit){
    unsigned long start_time = TS_Chrono_GetTime(unit);
    TS_Chrono_GC(ctx);
    unsigned long diff = time-(TS_Chrono_GetTime(unit)-start_time);
    TS_Chrono_SmallSleep(diff, unit);
}

unsigned long TS_Chrono_GetTime(enum TS_Chrono_TimeUnit unit){
    unsigned long long time = mach_absolute_time();
    unsigned long long nanotime = 0;
    
    if(timebase.denom==0)
        mach_timebase_info(&timebase);
    
     nanotime = time*timebase.numer/timebase.denom;
    
    return (unit==TS_Chrono_Millisecond)?(time/1000000):(time);
    
}
