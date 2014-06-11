#include "flipscreen.h"

#include "../loadfunc/thread_extra.h"

#include "../../../graphiccommon/screen.h"

#include <math.h>
#include <cassert>
#include <cstdio>

void * eng_frame = NULL;
void * gfx_frame = NULL;

void IncGFXFrame(void){
    Atomic::AtomicInc(gfx_frame);
}

void (*SwapBuffers)(void)       = NULL;
void (*SetFlipScreen)(void *)   = NULL;

static void(*OnClose)(void)     = NULL;

/////
// Full synchro for testing.

const int frame_float = 0;

void FlipScreen(void){
    TS_FlipScreen();

    assert(Atomic::AtomicGet(eng_frame)>=Atomic::AtomicGet(gfx_frame));
    if(frame_float>=0){
        while((Atomic::AtomicGet(eng_frame)-Atomic::AtomicGet(gfx_frame))>frame_float){
            //
        }
    }
}

void FlipScreenInit(bool threaded, bool composite){

    eng_frame = Atomic::CreateAtomic();
    gfx_frame = Atomic::CreateAtomic();

    Atomic::AtomicSet(eng_frame, 0);
    Atomic::AtomicSet(gfx_frame, 0);

    if(composite){
        FlipScreenCompositeInit(::GetScreenWidth(), ::GetScreenHeight());
        SwapBuffers = FlipScreenComposite;
        OnClose = FlipScreenCompositeClose;
    }
    else{
        FlipScreenDirectInit();
        SwapBuffers = FlipScreenDirect;
        OnClose = FlipScreenDirectClose;
    }

    if(threaded){
        SetFlipScreen = EnqueueFlipScreen;
    }
    else{
        SetFlipScreen = ImmediateFlipScreen;
    }


}

void TS_FlipScreen(){
    static Atomic::atomic_t atom = Atomic::CreateAtomic();
    Atomic::AtomicInc(eng_frame);
    SetFlipScreen(atom);
}

void FlipScreenClose(void){

    assert(OnClose!=NULL);

    if(OnClose)
        OnClose();
    else
        fprintf(stderr, "[" PLUGINNAME "] Error: FlipScreenClose called with no matching previous call to FlipScreenInit.\n");

    SwapBuffers     = NULL;
    OnClose         = NULL;
    SetFlipScreen   = NULL;

    Atomic::DestroyAtomic(eng_frame);
    Atomic::DestroyAtomic(gfx_frame);

}
