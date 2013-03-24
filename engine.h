#ifndef ENGINE_HEAD
#define ENGINE_HEAD
//extern v8::Persistent<v8::Context> context;

#include <string>
#include <math.h>
#if defined _WIN32
#include "stdafx.h"
#include "v8.h"
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_audio.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_thread.h"
#endif

#ifndef _WIN32

#include "v8.h"
#include "/usr/include/SDL/SDL.h"
#include "/usr/include/SDL/SDL_ttf.h"
#include "/usr/include/SDL/SDL_image.h"
#include "/usr/include/SDL/SDL_audio.h"
#include "/usr/include/SDL/SDL_mixer.h"
#include "/usr/include/SDL/SDL_thread.h"
#endif


#if defined _WIN32

#else

#endif


#endif
