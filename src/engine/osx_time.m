#include "osx_time.h"
#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>

static mach_timebase_info_data_t timebase;

void TS_OSX_Sleep(unsigned long time, enum TS_OSX_TimeUnit unit){

}

unsigned long TS_OSX_GetTime(enum TS_OSX_TimeUnit unit){
    unsigned long long time = mach_absolute_time();
    unsigned long long nanotime = 0;
    
    if(timebase.denom==0)
        mach_timebase_info(&timebase);
    
     nanotime = time*timebase.numer/timebase.denom;
    
    return (unit==TS_OSX_Millisecond)?(time/1000000):(time);
    
}