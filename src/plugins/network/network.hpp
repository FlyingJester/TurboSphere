#pragma once

#ifdef _WIN32
#define NETWORK_EXPORT __declspec(dllexport)
#define CCALL __cdecl
#endif
#ifndef _WIN32
#define CCALL
#define NETWORK_EXPORT
#endif

#define PLUGINNAME "Network"
#define BRACKNAME "[" PLUGINNAME "]"

#include <jsapi.h>
#include <pluginsdk/plugin.h>

extern "C" {

    NETWORK_EXPORT const char * CCALL Init(JSContext *ctx, unsigned ID);
    NETWORK_EXPORT void CCALL Close(JSContext *ctx);
    
    NETWORK_EXPORT int CCALL NumFunctions(JSContext *ctx);
    NETWORK_EXPORT JSNative CCALL GetFunction(JSContext *ctx, int n);
    NETWORK_EXPORT const char * CCALL GetFunctionName(JSContext *ctx, int n);
    
    NETWORK_EXPORT int CCALL NumVariables(JSContext *ctx);
    NETWORK_EXPORT void CCALL GetVariable(JSContext *ctx, int n, JS::MutableHandleValue val);
    NETWORK_EXPORT const char * CCALL GetVariableName(JSContext *ctx, int n);

}

