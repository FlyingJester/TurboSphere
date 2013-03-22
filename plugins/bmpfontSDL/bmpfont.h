#ifndef BMPFONT_HEAD
#define BMPFONT_HEAD

#include"../common/plugin.h"

#ifdef _WIN32
    #ifdef BMPFONT_INTERNAL
	#define BMPFONT_EXPORT __declspec(dllexport)
	#else
	#define BMPFONT_EXPORT __declspec(dllimport)
	#endif
	
    #define CCALL __cdecl

#else
#define CCALL 
#define BMPFONT_EXPORT extern "C"
#endif

#ifdef BMPFONT_INTERNAL
#ifdef _WIN32
	extern "C" {
#endif
BMPFONT_EXPORT void          CCALL Close(void);
BMPFONT_EXPORT initFunction  CCALL Init(void);
BMPFONT_EXPORT int           CCALL GetNumFunctions(void);
BMPFONT_EXPORT functionArray CCALL GetFunctions(void);
BMPFONT_EXPORT nameArray     CCALL GetFunctionNames(void);
BMPFONT_EXPORT int           CCALL GetNumVariables(void);
BMPFONT_EXPORT void **       CCALL GetVariables(void);
BMPFONT_EXPORT nameArray     CCALL GetVariableNames(void);
#ifdef _WIN32
	}
#endif
#endif
#endif