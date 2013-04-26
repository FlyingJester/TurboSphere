#ifndef SDL_GL_MAIN_HEAD
#define SDL_GL_MAIN_HEAD

#define PLUGINNAME "SDL_GL"
#include "../common/plugin.h"
#ifdef _WIN32
#include "../../SDL/SDL_opengl.h"
#else
#include <SDL/SDL_opengl.h>
#endif
#include "../common/graphic_algorithm.h"
#include "../common/graphic_common.h"
#include "../../configmanager/opengame.h"
#include <cmath>
#ifndef APIENTRY
#define APIENTRY
#endif

#ifdef _WIN32
	#define SDL_GL_EXPORT __declspec(dllexport)

    #define CCALL __cdecl

#endif
#ifndef _WIN32
#define CCALL
#define SDL_GL_EXPORT extern "C"
#endif

#define NUMFUNCS 17
#define NUMVARS  0

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
//this is not possible on proper platforms
//Except MIPS?
//You got this running on MIPS, I wanna know.
#else

    #define Frmask 0x000000ff
	#define Fgmask 0x0000ff00
	#define Fbmask 0x00ff0000
	#define Famask 0xff000000
#endif

#define IMG_FLAGS (IMG_INIT_JPG|IMG_INIT_PNG|IMG_INIT_TIF)
#define SDL_VIDEO_FLAGS SDL_HWSURFACE|SDL_GL_DOUBLEBUFFER|SDL_OPENGL

#define BPP 4
#define DEPTH 32

#define CHANNEL_MASKS Frmask, Fgmask, Fbmask, Famask

extern SDL_Rect cmpltscreen;

extern SDL_Event event;
extern SDL_Surface *screen;

#ifdef _WIN32
	extern "C" {
#endif

SDL_GL_EXPORT void            CCALL Close(void);
SDL_GL_EXPORT initFunction    CCALL Init(void);
SDL_GL_EXPORT int             CCALL GetNumFunctions(void);
SDL_GL_EXPORT functionArray   CCALL GetFunctions(void);
SDL_GL_EXPORT nameArray       CCALL GetFunctionNames(void);
SDL_GL_EXPORT int             CCALL GetNumVariables(void);
SDL_GL_EXPORT v8FunctionArray CCALL GetVariables(void);
SDL_GL_EXPORT nameArray       CCALL GetVariableNames(void);

#ifdef _WIN32
	}
#endif


#endif
