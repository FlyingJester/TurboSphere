#include "State.hpp"
#include "../Sapphire.hpp"

#ifdef OS_X
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "../Thread/Atomic.hpp"

/////
// These macros are horribly lossy. They are to compensate for when a high-
//   precision API is not known, and to adjust to make the lower-precision
//   value at least on the correct scale, or to help with combining lexically
//   separate time values.
//
// Always prefer to demote a more precise API than to promote a less precise
//   one.

#define MILLI_TO_MICRO(X) ((X)*1000)
#define MICRO_TO_NANO MILLI_TO_MICRO
#define MILLI_TO_NANO(X) (MICRO_TO_NANO(MILLI_TO_MICRO(X)))

#define MICRO_TO_MILLI(X) ((X)/1000)
#define NANO_TO_MICRO MICRO_TO_MILLI
#define NANO_TO_MILLI(X) (MICRO_TO_MILLI(NANO_TO_MICRO(X)))

#define SEC_TO_MILLI(X) ((X)*1000)
#define MILLI_TO_SEC(X) ((X)/1000)

#ifdef OS_X
#include <mach/mach.h>
#include <mach/mach_time.h>
static mach_timebase_info_data_t timebase;
#endif // OS_X

namespace Sapphire{
namespace Galileo{

unsigned long long GetMicroTime(){
#ifdef HAS_TIME // We want to use Unix+C time.h.

#ifdef _WIN32
#pragma warning Using clock_gettime on Windows.
#pragma warning You probably don't want to/didn't mean to do this.
#endif

#include <time.h>
#ifdef __linux__
  #define CLOCK_TYPE CLOCK_MONOTONIC
#else
  #define CLOCK_TYPE CLOCK_REALTIME
#endif

    struct timespec s;
    clock_gettime(CLOCK_TYPE, &s);
    return (SEC_TO_MILLI(MILLI_TO_MICRO(s.tv_sec))) + (NANO_TO_MICRO(s.tv_nsec));

#elif defined(OS_X) // Use MACH's bus timing.
    if(timebase.denom==0)
      mach_timebase_info(&timebase);

    assert(timebase.denom!=0);
    const unsigned long long lTime = mach_absolute_time();
    return NANO_TO_MICRO((lTime*timebase.numer)/timebase.denom);
#else // Use SDL2.

    return MILLI_TO_MICRO(SDL_GetTicks());

#endif
}

// Frames per second
static Sapphire::atomic32_t *sFPS = CreateAtomic(75);
// Micro-Sec Interval between frames for the current FPS.
static Sapphire::atomic32_t *sInterval = CreateAtomic(10000);
static Sapphire::atomic32_t *sLastFrame = CreateAtomic(0);

static bool sThrottled = false;

void SetFrameRate(int32_t aFramesPerSecond){
  if(aFramesPerSecond==0)
    sThrottled = false;
  else{
    sThrottled = true;
    AtomicSet(sFPS, aFramesPerSecond);
    AtomicSet(sInterval, MILLI_TO_NANO(SEC_TO_MILLI(1))/aFramesPerSecond);
  }
}

int32_t GetFrameRate(void){
  if(!sThrottled)
    return 0;
  return AtomicGet(sFPS);
}

FlipScreen::FlipScreen(SDL_Window *aWindow){

}

int FlipScreen::Draw(){

    SDL_Window *w = SDL_GL_GetCurrentWindow();

    SDL_GL_MakeCurrent(w, SDL_GL_GetCurrentContext());
    SDL_GL_SwapWindow(w);
    glClear(GL_COLOR_BUFFER_BIT);

    return 0;
}

bool ClippingRectangle::IsNecessary() const {
    return (x==0) && (y==0) && (w>=GetScreenWidth()) && (h>=GetScreenHeight());
}

int ClippingRectangle::Draw(){
    glScissor(x, y, w, h);
    return 0;
}

}
}
