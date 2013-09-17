#ifndef GKS_HEAD
#define GKS_HEAD

#ifdef _WIN32
	#define GKS_EXPORT __declspec(dllexport)
	
    #define CCALL __cdecl

#endif
#ifndef _WIN32
#define CCALL 
#define GKS_EXPORT extern "C"
#endif

#ifdef _WIN32
	extern "C" {
#endif
GKS_EXPORT void    CCALL Close(void);
GKS_EXPORT const char *  CCALL Init(void);
GKS_EXPORT int     CCALL GetNumFunctions(void);
GKS_EXPORT void ** CCALL GetFunctions(void);
GKS_EXPORT const char ** CCALL GetFunctionNames(void);
GKS_EXPORT int     CCALL GetNumVariables(void);
GKS_EXPORT void ** CCALL GetVariables(void);
GKS_EXPORT const char ** CCALL GetVariableNames(void);

#ifdef _WIN32
	}
#endif

#endif