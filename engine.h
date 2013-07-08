#ifndef ENGINE_HEAD
#define ENGINE_HEAD
//extern v8::Persistent<v8::Context> context;

#include <string>
#include <math.h>
#define V8_ALLOW_ACCESS_TO_RAW_HANDLE_CONSTRUCTOR
#define V8_USE_UNSAFE_HANDLES
#if defined _WIN32
#include "v8.h"
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_thread.h"
#error Fix these includes for SDL2!
    //TODO: Fix these includes for SDL2.
#endif

#ifndef _WIN32

#include "v8.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_thread.h>
#endif

#undef V8_ALLOW_ACCESS_TO_RAW_HANDLE_CONSTRUCTOR
#undef V8_USE_UNSAFE_HANDLES

#if defined _WIN32

#else

#endif


#endif
