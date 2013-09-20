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

void FlipScreen(void);


void ScreenInit(void);
void ScreenClose(void);

v8Function V8GetScreenWidth(V8ARGS);
v8Function V8GetScreenHeight(V8ARGS);

bool * const GetScreenShotFlag(void);

#endif
