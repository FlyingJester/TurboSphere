#ifndef AUDIO_BASS_FRONTEND_HEAD
#define AUDIO_BASS_FRONTEND_HEAD

#include "bass_backend.h"
#include "include/bass.h"
#define PLUGINNAME "audioBASS"
#include "../common/plugin.h"
#include "../../configmanager/opengame.h"

#define FAILSAFE_BASS_INIT_FLAGS (BASS_DEVICE_8BITS|BASS_DEVICE_MONO)

#ifdef _WIN32
	#define AUDIOBASS_EXPORT __declspec(dllexport)

    #define CCALL __cdecl

#endif
#ifndef _WIN32
#define CCALL
#define AUDIOBASS_EXPORT extern "C"
#endif

#define NUMFUNCS 2
#define NUMVARS  0

#ifdef _WIN32
	extern "C" {
#endif

AUDIOBASS_EXPORT void            CCALL Close(void);
AUDIOBASS_EXPORT initFunction    CCALL Init(void);
AUDIOBASS_EXPORT int             CCALL GetNumFunctions(void);
AUDIOBASS_EXPORT functionArray   CCALL GetFunctions(void);
AUDIOBASS_EXPORT nameArray       CCALL GetFunctionNames(void);
AUDIOBASS_EXPORT int             CCALL GetNumVariables(void);
AUDIOBASS_EXPORT v8FunctionArray CCALL GetVariables(void);
AUDIOBASS_EXPORT nameArray       CCALL GetVariableNames(void);

#ifdef _WIN32
	}
#endif

#endif
