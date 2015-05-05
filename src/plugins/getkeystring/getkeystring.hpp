#pragma once
#include <jsapi.h>

namespace GetKeyString{
    bool GetKeyString(JSContext *ctx, unsigned argc, JS::Value *vp);
}

#ifdef _WIN32
	#define GKS_EXPORT __declspec(dllexport)

    #define CCALL __cdecl

#endif
#ifndef _WIN32
#define CCALL
#define GKS_EXPORT
#endif

extern "C" {

    GKS_EXPORT const char * CCALL Init(JSContext *ctx, unsigned ID);
    GKS_EXPORT void CCALL Close(JSContext *ctx);
    
    GKS_EXPORT int CCALL NumFunctions(JSContext *ctx);
    GKS_EXPORT JSNative CCALL GetFunction(JSContext *ctx, int n);
    GKS_EXPORT const char * CCALL GetFunctionName(JSContext *ctx, int n);
    
    GKS_EXPORT int CCALL NumVariables(JSContext *ctx);
    GKS_EXPORT JS::Heap<JS::Value> *CCALL GetVariable(JSContext *ctx, int n);
    GKS_EXPORT const char * CCALL GetVariableName(JSContext *ctx, int n);

}
