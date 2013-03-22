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
#define NUMFUNCS 7

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
