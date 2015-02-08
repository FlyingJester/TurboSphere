#pragma once

#ifdef _WIN32
#define INPUTSDL2_EXPORT __declspec(dllexport)
#define CCALL __cdecl
#endif
#ifndef _WIN32
#define CCALL
#define INPUTSDL2_EXPORT
#endif

#define PLUGINNAME "InputSDL2"
#define BRACKNAME "[InputSDL2]"

#include <jsapi.h>
#include <pluginsdk/plugin.h>

extern "C" {

    INPUTSDL2_EXPORT const char * CCALL Init(JSContext *ctx, unsigned ID);
    INPUTSDL2_EXPORT void CCALL Close(JSContext *ctx);
    
    INPUTSDL2_EXPORT int CCALL NumFunctions(JSContext *ctx);
    INPUTSDL2_EXPORT JSNative CCALL GetFunction(JSContext *ctx, int n);
    INPUTSDL2_EXPORT const char * CCALL GetFunctionName(JSContext *ctx, int n);
    
    INPUTSDL2_EXPORT int CCALL NumVariables(JSContext *ctx);
    INPUTSDL2_EXPORT void CCALL GetVariable(JSContext *ctx, int n, JS::MutableHandleValue val);
    INPUTSDL2_EXPORT const char * CCALL GetVariableName(JSContext *ctx, int n);

}
