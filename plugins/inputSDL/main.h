#ifndef INPUTSDL_HEAD
#define INPUTSDL_HEAD

#ifdef _WIN32
    #ifdef INPUT_INTERNAL
	#define INPUTSDL_EXPORT __declspec(dllexport)
	#else
	#define INPUTSDL_EXPORT __declspec(dllimport)
	#endif

    #define CCALL __cdecl

#endif
#ifndef _WIN32
#define CCALL
#define INPUTSDL_EXPORT extern "C"
#endif

#include "../common/plugin.h"

#define NUMVARS 115
#define NUMFUNCS 13

//BIG UGLY HACK FOR SDL2 MIGRATION!
#define SDLK_NUMLOCK 0
#define SDLK_SCROLLOCK 0
#define SDLK_KP0 0
#define SDLK_KP1 0
#define SDLK_KP2 0
#define SDLK_KP3 0
#define SDLK_KP4 0
#define SDLK_KP5 0
#define SDLK_KP6 0
#define SDLK_KP7 0
#define SDLK_KP8 0
#define SDLK_KP9 0
#define SDLK_RMETA 0
#define SDLK_LMETA 0
#define SDLK_RSUPER 0
#define SDLK_LSUPER 0
#define SDLK_BREAK 0
#define SDLK_EURO 0

#ifdef _WIN32
	extern "C" {
#endif
INPUTSDL_EXPORT void          CCALL Close(void);
INPUTSDL_EXPORT initFunction  CCALL Init(void);
INPUTSDL_EXPORT int           CCALL GetNumFunctions(void);
INPUTSDL_EXPORT functionArray CCALL GetFunctions(void);
INPUTSDL_EXPORT nameArray     CCALL GetFunctionNames(void);
INPUTSDL_EXPORT int           CCALL GetNumVariables(void);
INPUTSDL_EXPORT v8FunctionArray CCALL GetVariables(void);
INPUTSDL_EXPORT nameArray     CCALL GetVariableNames(void);
#ifdef _WIN32
	}
#endif

extern SDL_Event keyevent;

#endif
