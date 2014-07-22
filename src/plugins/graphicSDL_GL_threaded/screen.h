#ifndef SDL_GL_SCREEN_HEAD
#define SDL_GL_SCREEN_HEAD
#include "main.h"

//TODO: Make these configurable.
#define DEFAULT_IMAGE_EXT ".tga"
#define SCREENSHOT_FOLDER "screenshots"

//I've heard this is different in the Bionic C library (android).
#if (defined _WIN32) || (defined _GNUC_)
    #define TM_YEAR_EPOCH 1900
#else
    #define TM_YEAR_EPOCH 0
#endif

extern void(* FlipScreen)(void);


void ScreenInit(void);
void ScreenClose(void);

TS_Segment TS_GetCLippingRectangle(void);
void TS_SetClippingRectangle(TS_Segment *segment);
v8Function SetClippingRectangle(V8ARGS);
v8Function GetClippingRectangle(V8ARGS);

v8Function V8FlipScreen(V8ARGS);
v8Function V8GetScreenWidth(V8ARGS);
v8Function V8GetScreenHeight(V8ARGS);

//Internal use only.
void FlipScreenComposite(void);
void FlipScreenDirect(void);

bool * const GetScreenShotFlag(void);

#endif
