#include <sys/time.h>
/* AIX and Solaris */

unsigned long long TS_Chrono_GetTime(enum TS_Chrono_TimeUnit unit){
    const static hrtime_t begin = gethrtime();
    const unsigned long diff = gethrtime() - begin;
    return (unit==TS_Chrono_Millisecond)?(time/1000000):(time);
}
