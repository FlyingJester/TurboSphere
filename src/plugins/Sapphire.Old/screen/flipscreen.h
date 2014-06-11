#ifndef SAPPHIRE_FLIPSCREEN_HEAD
#define SAPPHIRE_FLIPSCREEN_HEAD

#include "../libapi.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void IncGFXFrame(void);

extern void * eng_frame; // Number of calls to FlipScreen from the engine
extern void * gfx_frame; // Number of FlipScreens handled by Sapphire.
extern const int frame_float; //Number of frames out of sync the engine and the GFX can be.


/////
// Initialization

void FlipScreenCompositeInit(int w, int h);
void FlipScreenDirectInit(void);
void FlipScreenCompositeClose(void);
void FlipScreenDirectClose(void);

void FlipScreenInit(bool threaded, bool composite);
void FlipScreenClose(void);

/////
// Function that handles all configurations:

void TS_FlipScreen(void);

// The rest of this file is internal to Sapphire's screen components.

/////
// FlipScreen Calls

void FlipScreenDirect(void);
void FlipScreenComposite(void);

extern void (*SwapBuffers)(void);

/////
// Threading-model Calls

void EnqueueFlipScreen(void *atomic_flag);
void ImmediateFlipScreen(void *unused);

extern void (*SetFlipScreen)(void *);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include "../backend.h"
#include "../../common/plugin.h"

namespace Script{

    /////
    // Flipping and Frame Rate

    Turbo::JSFunction FlipScreen(Turbo::JSArguments args);
    Turbo::JSFunction GetFrameRate(Turbo::JSArguments args);
    Turbo::JSFunction SetFrameRate(Turbo::JSArguments args);

    /////
    // Clipping Rectangle

    Turbo::JSFunction GetClippingRectangle(Turbo::JSArguments args);
    Turbo::JSFunction SetClippingRectangle(Turbo::JSArguments args);
    Turbo::JSFunction GetScreenWidth(Turbo::JSArguments args);
    Turbo::JSFunction GetScreenHeight(Turbo::JSArguments args);

}


#endif

#endif
