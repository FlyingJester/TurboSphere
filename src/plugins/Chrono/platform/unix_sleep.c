#include "sm_wrapper.h"
#include "time.h"
#include <unistd.h>

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
