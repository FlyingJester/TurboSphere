#ifndef OSX_TIME_HEAD
#define OSX_TIME_HEAD

/////
// Simple time routines from OS X, wrapped for C/C++ usage.
//
// Designed for use by Delay() and GetTime() in script. Makes
// sure we have a replacement for SDL_Delay and SDL_GetTicks
// when SDL is not available.
//

#ifdef __cplusplus
    #define OBJCEXTERN extern "C"
    #define OBJCDEFARGS(x) = x
#else
#define OBJCEXTERN
#define OBJCDEFARGS(x)
#endif

enum TS_OSX_TimeUnit {TS_OSX_Millisecond, TS_OSX_Nanosecond};

OBJCEXTERN void TS_OSX_Sleep(unsigned long time, enum TS_OSX_TimeUnit unit OBJCDEFARGS(TS_OSX_Millisecond));
OBJCEXTERN unsigned long TS_OSX_GetTime(enum TS_OSX_TimeUnit unit OBJCDEFARGS(TS_OSX_Millisecond));

#endif