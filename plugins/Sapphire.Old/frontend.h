#ifndef SAPPHIRE_FRONTEND_HEAD
#define SAPPHIRE_FRONTEND_HEAD

#define PLUGINNAME "Sapphire"
#ifdef __cplusplus
#include "../common/plugin.h"
#endif

#ifdef _WIN32
	#define SAPPHIRE_EXPORT __declspec(dllexport)

    #define CCALL __cdecl

#endif
#ifndef _WIN32
#define CCALL
#ifdef __cplusplus
#define SAPPHIRE_EXPORT extern "C"
#else
#define SAPPHIRE_EXPORT
#endif
#endif

#define NUMFUNCS 0
#define NUMVARS  0

#ifdef __cplusplus
extern v8::Isolate *iso;

    #ifdef _WIN32
        extern "C" {
    #endif

    SAPPHIRE_EXPORT void                   CCALL Close(void);
    SAPPHIRE_EXPORT const char *           CCALL Init(int);
    SAPPHIRE_EXPORT int                    CCALL GetNumFunctions(void);
    SAPPHIRE_EXPORT Turbo::JSFunctionArray CCALL GetFunctions(void);
    SAPPHIRE_EXPORT Turbo::JSNameArray     CCALL GetFunctionNames(void);
    SAPPHIRE_EXPORT int                    CCALL GetNumVariables(void);
    SAPPHIRE_EXPORT Turbo::JSValueArray    CCALL GetVariables(void);
    SAPPHIRE_EXPORT Turbo::JSNameArray     CCALL GetVariableNames(void);

    #ifdef _WIN32
        }
    #endif
#else

#endif

#endif
