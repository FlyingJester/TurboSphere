#ifndef TS_NETWORK_HEAD
#define TS_NETWORK_HEAD

    //Returns the network name of the current machine. Returns null on error.
const char *TS_GetHostName(void);

    //This function was for Android originally. It is mostly unnecessary, but is included for the possiblity of a
    //future Android release, or for porting to other platforms were it may be useful.
inline int TS_FreeHostName(const char *);

#ifdef _WIN32
	#define NETWORKTS_EXPORT __declspec(dllexport)

    #define CCALL __cdecl

#endif
#ifdef _WIN32
#include <cstdlib>
inline int TS_FreeHostName(const char * name){
    free((void *)name);
    return 0;
}
#else


#include <cstdlib>
inline int TS_FreeHostName(const char * name){
    free((void *)name);
    return 0;
}


#define CCALL
#define NETWORKTS_EXPORT extern "C"
#endif

#ifdef NETWORKTS_INTERNAL
#ifdef _WIN32
	extern "C" {
#endif
NETWORKTS_EXPORT void          CCALL Close(void);
NETWORKTS_EXPORT initFunction  CCALL Init(void);
NETWORKTS_EXPORT int           CCALL GetNumFunctions(void);
NETWORKTS_EXPORT functionArray CCALL GetFunctions(void);
NETWORKTS_EXPORT nameArray     CCALL GetFunctionNames(void);
NETWORKTS_EXPORT int           CCALL GetNumVariables(void);
NETWORKTS_EXPORT void **       CCALL GetVariables(void);
NETWORKTS_EXPORT nameArray     CCALL GetVariableNames(void);
#ifdef _WIN32
	}
#endif
#endif

#endif
