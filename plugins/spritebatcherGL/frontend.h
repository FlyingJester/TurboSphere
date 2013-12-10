#ifndef SPRITEBATCHER_FRONTEND_HEAD
#define SPRITEBATCHER_FRONTEND_HEAD

#define PLUGINNAME "spritebatcherGL"

#include "../common/plugin.h"

#ifdef _WIN32
	#define SPRITEBATCHER_EXPORT __declspec(dllexport)

    #define CCALL __cdecl

#endif
#ifndef _WIN32
#define CCALL
#define SPRITEBATCHER_EXPORT extern "C"
#endif


#ifdef _WIN32
	extern "C" {
#endif
SPRITEBATCHER_EXPORT void            CCALL GetPluginInfo(TS_PluginInfo *info);

SPRITEBATCHER_EXPORT void            CCALL Close(void);
SPRITEBATCHER_EXPORT initFunction    CCALL Init(int);
SPRITEBATCHER_EXPORT int             CCALL GetNumFunctions(void);
SPRITEBATCHER_EXPORT functionArray   CCALL GetFunctions(void);
SPRITEBATCHER_EXPORT nameArray       CCALL GetFunctionNames(void);
SPRITEBATCHER_EXPORT int             CCALL GetNumVariables(void);
SPRITEBATCHER_EXPORT v8FunctionArray CCALL GetVariables(void);
SPRITEBATCHER_EXPORT nameArray       CCALL GetVariableNames(void);

#ifdef _WIN32
	}
#endif

#endif  //SPRITEBATCHER_HEAD

