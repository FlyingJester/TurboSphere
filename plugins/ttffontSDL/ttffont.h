#ifndef BMPFONT_HEAD
#define BMPFONT_HEAD

#include"../common/plugin.h"

#ifdef _WIN32
    #ifdef TTFFONT_INTERNAL
	#define TTFFONT_EXPORT __declspec(dllexport)
	#else
	#define TTFFONT_EXPORT __declspec(dllimport)
	#endif
	
    #define CCALL __cdecl

#else
#define CCALL 
#define TTFFONT_EXPORT extern "C"
#endif

#ifdef TTFFONT_INTERNAL
#ifdef _WIN32
	extern "C" {
#endif
TTFFONT_EXPORT void          CCALL Close(void);
TTFFONT_EXPORT initFunction  CCALL Init(void);
TTFFONT_EXPORT int			 CCALL GetNumCategories(void);
TTFFONT_EXPORT nameArray	 CCALL GetCategories(void);
TTFFONT_EXPORT int           CCALL GetNumFunctions(void);
TTFFONT_EXPORT functionArray CCALL GetFunctions(void);
TTFFONT_EXPORT nameArray     CCALL GetFunctionNames(void);
TTFFONT_EXPORT int           CCALL GetNumVariables(void);
TTFFONT_EXPORT void **       CCALL GetVariables(void);
TTFFONT_EXPORT nameArray     CCALL GetVariableNames(void);
#ifdef _WIN32
	}
#endif
#endif
#endif