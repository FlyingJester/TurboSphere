#pragma once

#define PLUGINNAME "bmpfont"
#define BRACKNAME "[" PLUGINNAME "]"

#ifdef _WIN32
    #ifdef BMPFONT_INTERNAL
	#define BMPFONT_EXPORT __declspec(dllexport)
	#else
	#define BMPFONT_EXPORT __declspec(dllimport)
	#endif

    #define CCALL __cdecl

#endif
#ifndef _WIN32
#define CCALL
#define BMPFONT_EXPORT extern "C"
#endif


#include <pluginsdk/plugin.h>

extern "C" {
BMPFONT_EXPORT void                         CCALL Close(void);
BMPFONT_EXPORT const char *                 CCALL Init(int);
BMPFONT_EXPORT int                          CCALL GetNumFunctions(void);
BMPFONT_EXPORT const Turbo::JSFunctionArray CCALL GetFunctions(void);
BMPFONT_EXPORT const Turbo::JSNameArray     CCALL GetFunctionNames(void);
BMPFONT_EXPORT int                          CCALL GetNumVariables(void);
BMPFONT_EXPORT const Turbo::JSValueArray    CCALL GetVariables(void);
BMPFONT_EXPORT const Turbo::JSNameArray     CCALL GetVariableNames(void);
}
