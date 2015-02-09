#pragma once

#ifdef _WIN32
	#define CHRONO_EXPORT __declspec(dllexport)

    #define CCALL __cdecl

#endif
#ifndef _WIN32
#define CCALL
#define CHRONO_EXPORT
#endif

#define PLUGINNAME "Chrono"
#define BRACKNAME "[Chrono]"

#include <jsapi.h>
#include <pluginsdk/plugin.h>

extern "C" {

    CHRONO_EXPORT const char * CCALL Init(JSContext *ctx, unsigned ID);
    CHRONO_EXPORT void CCALL Close(JSContext *ctx);
    
    CHRONO_EXPORT int CCALL NumFunctions(JSContext *ctx);
    CHRONO_EXPORT JSNative CCALL GetFunction(JSContext *ctx, int n);
    CHRONO_EXPORT const char * CCALL GetFunctionName(JSContext *ctx, int n);
    
    CHRONO_EXPORT int CCALL NumVariables(JSContext *ctx);
    CHRONO_EXPORT JS::Heap<JS::Value> *CCALL GetVariable(JSContext *ctx, int n);
    CHRONO_EXPORT const char * CCALL GetVariableName(JSContext *ctx, int n);

}
