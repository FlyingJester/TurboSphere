#pragma once

#ifdef _WIN32
#define NETPLUG_EXPORT __declspec(dllexport)
#define CCALL __cdecl
#endif
#ifndef _WIN32
#define CCALL
#define NETPLUG_EXPORT
#endif

#define PLUGINNAME "NetPlug"
#define BRACKNAME "[" PLUGINNAME "]"

#include <jsapi.h>
#include <pluginsdk/plugin.h>

extern "C" {

    NETPLUG_EXPORT const char * CCALL Init(JSContext *ctx, unsigned ID);
    NETPLUG_EXPORT void CCALL Close(JSContext *ctx);
    
    NETPLUG_EXPORT int CCALL NumFunctions(JSContext *ctx);
    NETPLUG_EXPORT JSNative CCALL GetFunction(JSContext *ctx, int n);
    NETPLUG_EXPORT const char * CCALL GetFunctionName(JSContext *ctx, int n);
    
    NETPLUG_EXPORT int CCALL NumVariables(JSContext *ctx);
    NETPLUG_EXPORT void CCALL GetVariable(JSContext *ctx, int n, JS::MutableHandleValue val);
    NETPLUG_EXPORT const char * CCALL GetVariableName(JSContext *ctx, int n);

}

