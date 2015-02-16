#pragma once

#ifdef _WIN32
#define CINNAMON_EXPORT __declspec(dllexport)
#define CCALL __cdecl
#endif
#ifndef _WIN32
#define CCALL
#define CINNAMON_EXPORT
#endif

#define PLUGINNAME "Cinnamon"
#define BRACKNAME "[" PLUGINNAME "]"

#include <jsapi.h>
#include <pluginsdk/plugin.h>

extern "C" {

    CINNAMON_EXPORT const char * CCALL Init(JSContext *ctx, unsigned ID);
    CINNAMON_EXPORT void CCALL Close(JSContext *ctx);
    
    CINNAMON_EXPORT int CCALL NumFunctions(JSContext *ctx);
    CINNAMON_EXPORT JSNative CCALL GetFunction(JSContext *ctx, int n);
    CINNAMON_EXPORT const char * CCALL GetFunctionName(JSContext *ctx, int n);
    
    CINNAMON_EXPORT int CCALL NumVariables(JSContext *ctx);
    CINNAMON_EXPORT void CCALL GetVariable(JSContext *ctx, int n, JS::MutableHandleValue val);
    CINNAMON_EXPORT const char * CCALL GetVariableName(JSContext *ctx, int n);

}

