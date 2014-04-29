#ifdef HAS_ATOMIC
#include "drawqueue.hpp"

#include <cinttypes>
#include <atomic>

typedef std::atomic_uint_fast32_t FJ_Atomic;
static FJ_Atomic engine_frame, render_frame, thread_die;

void IncRenderFrame(void){
    render_frame.fetch_add(1, std::memory_order::memory_order_relaxed);
}

void IncEngineFrame(void){
    engine_frame.fetch_add(1, std::memory_order::memory_order_relaxed);
}

uint32_t GetRenderFrame(void){
    return render_frame.load();
}

uint32_t GetEngineFrame(void){
    return engine_frame.load();
}

void ResetRenderFrame(void){
    render_frame = 0;
}

void ResetEngineFrame(void){
    engine_frame = 0;
}

void SetThreadDie(int v){
    thread_die = v;
}

int  GetThreadDie(void){
    return thread_die;
}

#endif
