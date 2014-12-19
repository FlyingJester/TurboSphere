#pragma once

#define PLUGINNAME "Cinnamon"
#define BRACKNAME "[" PLUGINNAME "]"

#include <pluginsdk/plugin.h>

#define NUM_FUNCS 1
#define NUM_VARS  2

#ifdef _WIN32
	#define CINNAMON_EXPORT __declspec(dllexport)
  #define CCALL __cdecl
#endif

#ifndef _WIN32
  #define CCALL
  #define CINNAMON_EXPORT extern "C"
#endif

#ifdef _WIN32
	extern "C" {
#endif
    CINNAMON_EXPORT void                   CCALL Close(void);
    CINNAMON_EXPORT const char *           CCALL Init(int ID);
    CINNAMON_EXPORT int                    CCALL GetNumFunctions(void);
    CINNAMON_EXPORT Turbo::JSFunctionArray CCALL GetFunctions(void);
    CINNAMON_EXPORT Turbo::JSNameArray     CCALL GetFunctionNames(void);
    CINNAMON_EXPORT int                    CCALL GetNumVariables(void);
    CINNAMON_EXPORT Turbo::JSValueArray    CCALL GetVariables(void);
    CINNAMON_EXPORT Turbo::JSNameArray     CCALL GetVariableNames(void);
#ifdef _WIN32
	}
#endif
