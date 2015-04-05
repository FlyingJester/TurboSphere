#pragma once

#ifdef _WIN32
#define FS_EXPORT __declspec(dllexport)
#define CCALL __cdecl
#endif
#ifndef _WIN32
#define CCALL
#define FS_EXPORT
#endif

#define PLUGINNAME "FS"
#define BRACKNAME "[" PLUGINNAME "]"

#include <jsapi.h>
#include <pluginsdk/plugin.h>

extern "C" {

    FS_EXPORT const char * CCALL Init(JSContext *ctx, unsigned ID);
    FS_EXPORT void CCALL Close(JSContext *ctx);
    
    FS_EXPORT int CCALL NumFunctions(JSContext *ctx);
    FS_EXPORT JSNative CCALL GetFunction(JSContext *ctx, int n);
    FS_EXPORT const char * CCALL GetFunctionName(JSContext *ctx, int n);
    
    FS_EXPORT int CCALL NumVariables(JSContext *ctx);
    FS_EXPORT void CCALL GetVariable(JSContext *ctx, int n, JS::MutableHandleValue val);
    FS_EXPORT const char * CCALL GetVariableName(JSContext *ctx, int n);

}

