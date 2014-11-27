
#include "time.hpp"
#include <v8.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#include <ctime>

#ifdef _WIN32

#include <mmsystem.h>

//Get Sleep function
#if NTDDI_VERSION == NTDDI_WIN8 //As much as I am loathe to do this.
    #include <Synchapi.h>
#else
    #include <WinBase.h>
#endif

#define STRDUP _strdup
#define snprintf _snprintf
#if _MSC_VER <= 1600
	#define isblank(x) ((x==0x9)||(x==0x20))
	#define isprint(x) ((x>=0x20)&&(x<0x7F))
#endif
#else

#ifdef HAS_UNISTD_SYS
#include <sys/unistd.h>
    #define TS_SLEEP(x) usleep(x*1000)
#elif defined HAS_UNISTD
#include <unistd.h>
    #define TS_SLEEP(x) usleep(x*1000)
#endif

#include <sys/time.h>
#define STRDUP strdup
#endif

#if ( defined __MACH__ ) && (defined __APPLE__ )
    #include "osx_time.h"

    #define OS_X 1
#endif

double TS_GetSeconds(){
    #ifdef _WIN32
    return ((double)timeGetTime()))/1000.0
    #elif defined OS_X
    return ((double)TS_OSX_GetTime(TS_OSX_TimeUnit::TS_OSX_Nanosecond))/1000000000.0;
    #else
    struct timespec s;

    clock_gettime(CLOCK_MONOTONIC, &s);

    return ((double)((s.tv_sec*1000)+(s.tv_nsec/1000000)))/1000.0;
    #endif // _WIN32
}

namespace Turbo {

void Delay(const v8::FunctionCallbackInfo<v8::Value> &args){
	unsigned int t = args[0]->Uint32Value();

	#ifdef _WIN32

	SDL_Delay(t);

  #else
    static uint32_t max = 0;

    if(t>max){
    //If we are delaying for longer than the longest time it ever took to call v8::V8::LowMemoryNotification(),
    //Then we will call it and subtract the time difference it made from the ms to sleep for.

    #ifdef OS_X
        uint32_t ot = TS_OSX_GetTime();
    #else
        clock_gettime(CLOCK_MONOTONIC, &s);
        uint32_t ot = (s.tv_sec*1000)+(s.tv_nsec/1000000);
    #endif

    GarbageCollect(args);

    // Sapphire broke this. Well, actually, Sapphire is broken, and this shows it.
        //while(!v8::V8::IdleNotification(1000)){}

        //v8::V8::LowMemoryNotification();

    #ifdef OS_X
        ot = TS_OSX_GetTime()-ot;
    #else
        clock_gettime(CLOCK_MONOTONIC, &s);
        ot = (s.tv_sec*1000)+(s.tv_nsec/1000000)-ot;
    #endif

        t-=ot;
        if(ot>max){
            max=ot;
            printf("[Engine] Info: Increased minimum time threshold to %i.\n", max);
        }
    }

    TS_SLEEP(t);
  //  #else

  /*
    //If we are delaying for longer than the longest time it ever took to call v8::V8::LowMemoryNotification(),
    //Then we will call it and subtract the time difference it made from the ms to sleep for.
    static uint32_t max = 0;
    if(t>max){

        struct timespec s;

        clock_gettime(CLOCK_MONOTONIC, &s);
        uint32_t ot = (s.tv_sec*1000)+(s.tv_nsec/1000000);

        while(!v8::V8::IdleNotification(1000)){}

        v8::V8::LowMemoryNotification();


        clock_gettime(CLOCK_MONOTONIC, &s);
        ot = (s.tv_sec*1000)+(s.tv_nsec/1000000)-ot;
        t-=ot;
        if(ot>max){
            max=ot;
            printf(ENGINE " Info: Increased minimum time threshold to %i.\n", max);
        }
    }

        TS_SLEEP(t);
        */
    #endif  //_WIN32

}


// Gets Time down to Microsecond accuracy.
void GetSeconds(const v8::FunctionCallbackInfo<v8::Value> &args){
    args.GetReturnValue().Set(v8::Number::New(args.GetIsolate(), TS_GetSeconds()));
}

void GetTime(const v8::FunctionCallbackInfo<v8::Value> &args){
    args.GetReturnValue().Set(v8::Number::New(args.GetIsolate(), TS_GetSeconds()*1000.0));
}

}
