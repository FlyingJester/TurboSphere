#ifndef SAPPHIRE_FRONTEND_HEAD
#define SAPPHIRE_FRONTEND_HEAD

#define PLUGINNAME "Sapphire"
#include "../common/plugin.h"


#ifdef _WIN32
	#define SAPPHIRE_EXPORT __declspec(dllexport)

    #define CCALL __cdecl

#endif
#ifndef _WIN32
#define CCALL
#define SAPPHIRE_EXPORT extern "C"
#endif

#define NUMFUNCS 0
#define NUMVARS  0

extern v8::Isolate *iso;

#ifdef _WIN32
	extern "C" {
#endif

SAPPHIRE_EXPORT void                   CCALL Close(void);
SAPPHIRE_EXPORT const char *           CCALL Init(void);
SAPPHIRE_EXPORT int                    CCALL GetNumFunctions(void);
SAPPHIRE_EXPORT Turbo::JSFunctionArray CCALL GetFunctions(void);
SAPPHIRE_EXPORT Turbo::JSNameArray     CCALL GetFunctionNames(void);
SAPPHIRE_EXPORT int                    CCALL GetNumVariables(void);
SAPPHIRE_EXPORT Turbo::JSValueArray    CCALL GetVariables(void);
SAPPHIRE_EXPORT Turbo::JSNameArray     CCALL GetVariableNames(void);

#ifdef _WIN32
	}
#endif

#endif
