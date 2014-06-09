#if (defined USE_SDL2_ATOMIC) || (!defined HAS_ATOMIC)

#ifdef HAS_ATOMIC
    #ifdef _MSC_VER
        #pragma warning Using SDL atomics, even though C++11 atomics are available.
    #else
        #warning Using SDL atomics, even though C++11 atomics are available.
    #endif
#endif

#include "drawqueue.hpp"
#include <SDL2/SDL_atomic.h>

typedef SDL_atomic_t TS_Atomic;
static TS_Atomic engine_frame, render_frame, thread_die;

void IncRenderFrame(void){
    SDL_AtomicAdd(&render_frame, 1);
}

void IncEngineFrame(void){
    SDL_AtomicAdd(&engine_frame, 1);
}

uint32_t GetRenderFrame(void){
    return SDL_AtomicGet(&render_frame);
}

uint32_t GetEngineFrame(void){
    return SDL_AtomicGet(&engine_frame);
}

void ResetRenderFrame(void){
    SDL_AtomicSet(&render_frame, 0);
}

void ResetEngineFrame(void){
    SDL_AtomicSet(&engine_frame, 0);
}

void SetThreadDie(int v){
    SDL_AtomicSet(&thread_die, v);
}

int  GetThreadDie(void){
    return SDL_AtomicGet(&thread_die);
}

#endif
