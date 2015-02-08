#pragma once

#include "Sapphire.hpp"


#ifdef _WIN32
	#define SAPPHIRE_EXPORT __declspec(dllexport)

    #define CCALL __cdecl

#endif
#ifndef _WIN32
#define CCALL
#define SAPPHIRE_EXPORT
#endif

#define CROSSPLUGIN_EXPORT SAPPHIRE_EXPORT

#include <jsapi.h>
#include <pluginsdk/plugin.h>


extern "C" {

    SAPPHIRE_EXPORT const char * CCALL Init(JSContext *ctx, unsigned ID);
    SAPPHIRE_EXPORT void CCALL Close(JSContext *ctx);
    
    SAPPHIRE_EXPORT int CCALL NumFunctions(JSContext *ctx);
    SAPPHIRE_EXPORT JSNative CCALL GetFunction(JSContext *ctx, int n);
    SAPPHIRE_EXPORT const char * CCALL GetFunctionName(JSContext *ctx, int n);
    
    SAPPHIRE_EXPORT int CCALL NumVariables(JSContext *ctx);
    SAPPHIRE_EXPORT JS::Heap<JS::Value> *CCALL GetVariable(JSContext *ctx, int n);
    SAPPHIRE_EXPORT const char * CCALL GetVariableName(JSContext *ctx, int n);

}
