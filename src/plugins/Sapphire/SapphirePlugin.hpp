#pragma once

#include "Sapphire.hpp"

#include <pluginsdk/plugin.h>

#ifdef _WIN32
	#define SAPPHIRE_EXPORT __declspec(dllexport)

    #define CCALL __cdecl

#else

#define CCALL
#define SAPPHIRE_EXPORT extern "C"
#endif

#define CROSSPLUGIN_EXPORT SAPPHIRE_EXPORT

#ifdef _WIN32
	extern "C" {
#endif
SAPPHIRE_EXPORT void                          CCALL Close(void);
SAPPHIRE_EXPORT const char *                  CCALL Init(int);
SAPPHIRE_EXPORT int                           CCALL GetNumFunctions(void);
SAPPHIRE_EXPORT const Turbo::JSFunctionArray  CCALL GetFunctions(void);
SAPPHIRE_EXPORT const Turbo::JSNameArray      CCALL GetFunctionNames(void);
SAPPHIRE_EXPORT int                           CCALL GetNumVariables(void);
SAPPHIRE_EXPORT const Turbo::JSValueArray     CCALL GetVariables(void);
SAPPHIRE_EXPORT const Turbo::JSNameArray      CCALL GetVariableNames(void);
#ifdef _WIN32
	}
#endif

