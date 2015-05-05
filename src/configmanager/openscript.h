#pragma once
#include <string>
#include <vector>
#include <jsapi.h>
#include "opengame.h"

#ifdef _WIN32
	#ifdef CONFIGMGR_INTERNAL
		#define CONFIGMGR_EXPORT __declspec(dllexport)
	#else
		#define CONFIGMGR_EXPORT __declspec(dllimport)
	#endif
#else
	#define CONFIGMGR_EXPORT extern "C"
#endif

bool CONFIGMGR_EXPORT TS_ExecuteString(JSContext *ctx, const char *filename, const char *source);
bool CONFIGMGR_EXPORT TS_ExecuteStringL(JSContext *ctx, const char *filename, const char *source, size_t len);
bool CONFIGMGR_EXPORT TS_LoadScript(JSContext *ctx, const char *filename, bool only_once = false);

// only_once=true for Require*Script, only_once=false for Evaluate*Script
bool CONFIGMGR_EXPORT TS_LoadScriptPrefixed_JS(JSContext *ctx, unsigned argc, JS::Value *vp, const char * prefix, bool only_once = true);

template<bool only_once>
bool TS_LoadScript_JS(JSContext *ctx, unsigned argc, JS::Value *vp){
    return TS_LoadScriptPrefixed_JS(ctx, argc, vp, TS_GetContextEnvironment(ctx)->directories->script, only_once);
}

template<bool only_once>
bool TS_LoadSystemScript_JS(JSContext *ctx, unsigned argc, JS::Value *vp){
    return TS_LoadScriptPrefixed_JS(ctx, argc, vp, TS_GetContextEnvironment(ctx)->system->systemscript, only_once);
}

