#include "flipscreen.hpp"
#include "glextra/gl_extra.h"
#include <math.h>

#include <cassert>

#ifdef HAS_UNISTD_SYS
    #include <sys/unistd.h>
#elif defined HAS_UNISTD
    #include <unistd.h>
#endif

#if ( defined HAS_UNISTD ) || (defined HAS_UNISTD_SYS)

    typedef unsigned long long TS_Tick;

    static const TS_Tick microseconds_per_tick = 1;

    template <typename t>
    static inline void MicroSleep(t x){
        usleep(x);
    }

    static inline void NaturalSleep(unsigned micro){
        usleep(micro);
    }

    template<typename t>
    static inline t natural_ticks_from_microseconds(t ticks){
        return ticks;
    }

    static inline TS_Tick GetTicks(){
        return clock();
    }

#else
    #include <SDL2/SDL.h>
    typedef unsigned long TS_Tick;
    static TS_Tick microseconds_per_tick = 1000;

    template<typename t>
    static inline t natural_ticks_from_microseconds(t ticks){
        return ticks/((t)1000);
    }

    template <typename t>
    static inline void MicroSleep(t x){
        if(x<1000){
            SDL_Delay(1);
            return;
        }
        SDL_Delay(x/((t)1000));
    }

    static inline void NaturalSleep(unsigned milli){
        MicroSleep(milli*1000);
    }

    static inline TS_Tick GetTicks(){
        return SDL_GetTicks();
    }

#endif

static inline unsigned long milliseconds_from_ticks(TS_Tick ticks){
    return (ticks*microseconds_per_tick)/1000;
}

static inline TS_Tick natural_ticks_from_fps(unsigned long fps){
    return natural_ticks_from_microseconds(((TS_Tick)1000000)/fps);
}

namespace screen_secret{
    SDL_Window *window = NULL;
    bool throttled     = false;
    unsigned long framerate = 60;

    TS_Tick ticks_per_frame = natural_ticks_from_fps(framerate);

    // Access only from GL thread.
    TS_Tick LastTick = 0;
}

void Sapphire::FlipScreen(void){
    Sapphire::DrawingOp *fs_op = NULL;
    // Mercifully, since the throttled/unthrottled state is a part of the op,
    //   it is pipelined directly into the draw queue.
    if(screen_secret::throttled){
        fs_op = new Sapphire::Throttled::Direct::FlipScreenOp();
    }
    else{
        fs_op = new Sapphire::Unthrottled::Direct::FlipScreenOp();
    }

    assert(fs_op!=NULL);

    PushDrawingOp(fs_op);
}

void Sapphire::SetWindowToFlip(SDL_Window *_window){
    screen_secret::window = _window;
}

int Sapphire::UnthrottledDirectFlipScreenOp::call(void){
    SDL_GL_SwapWindow(screen_secret::window);
    IncRenderFrame();
    glClear(GL_COLOR_BUFFER_BIT);
    return 1;
}

int Sapphire::UnthrottledCompositeFlipScreenOp::call(void){
    Sapphire::ThrowInternalError("Compositing with Threading is not yet implemented. Adjust your settings.");
    return 0;
}
int Sapphire::ThrottledDirectFlipScreenOp::call(void){

    //Wait until enough ticks have occured to not exceed the requested FPS.
    if(GetTicks()<screen_secret::LastTick+screen_secret::ticks_per_frame){
        NaturalSleep(GetTicks()-screen_secret::LastTick+screen_secret::ticks_per_frame);
    }

    SDL_GL_SwapWindow(screen_secret::window);
    IncRenderFrame();
    glClear(GL_COLOR_BUFFER_BIT);
    return 1;
}

int Sapphire::ThrottledCompositeFlipScreenOp::call(void){
    Sapphire::ThrowInternalError("Compositing with Threading is not yet implemented. Adjust your settings.");
    return 0;
}
