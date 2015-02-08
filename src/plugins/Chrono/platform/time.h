#pragma once

/////
// Simple time routines from OS X, wrapped for C/C++ usage.
//
// Designed for use by Delay() and GetTime() in script. Makes
// sure we have a replacement for SDL_Delay and SDL_GetTicks
// when SDL is not available.
//

#ifdef __cplusplus
#define CHRONO_PLATFORM_DEFARGS(x) = x
extern "C" {
#else
#define CHRONO_PLATFORM_DEFARGS(x)
#endif

    enum TS_Chrono_TimeUnit {TS_Chrono_Millisecond, TS_Chrono_Nanosecond};

    void TS_Chrono_SmallSleep(unsigned long time, enum TS_Chrono_TimeUnit unit CHRONO_PLATFORM_DEFARGS(TS_Chrono_Millisecond));
    void TS_Chrono_Sleep(void *ctx, unsigned long time, enum TS_Chrono_TimeUnit unit CHRONO_PLATFORM_DEFARGS(TS_Chrono_Millisecond));
    unsigned long TS_Chrono_GetTime(enum TS_Chrono_TimeUnit unit CHRONO_PLATFORM_DEFARGS(TS_Chrono_Millisecond));

#ifdef __cplusplus
}
#endif
