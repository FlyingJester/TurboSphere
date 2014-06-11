#ifndef GRAPHIC_COMMON_SCREEN_HEAD
#define GRAPHIC_COMMON_SCREEN_HEAD

#ifdef _WIN32
	#ifdef GRAPHICC_INTERNAL
		#define GRAPHICC_EXPORT __declspec(dllexport)
	#else
		#define GRAPHICC_EXPORT __declspec(dllimport)
	#endif
#define GRAPHICCCALL _cdecl
#else
    #ifdef __cplusplus
        #define GRAPHICC_EXPORT extern "C"
    #else
        #define GRAPHICC_EXPORT
    #endif
#endif

GRAPHICC_EXPORT unsigned int GetScreenWidth(void);
GRAPHICC_EXPORT unsigned int GetScreenHeight(void);
GRAPHICC_EXPORT int SetScreenWidth(unsigned int);
GRAPHICC_EXPORT int SetScreenHeight(unsigned int);
GRAPHICC_EXPORT unsigned int GetMinScreenWidth(void);
GRAPHICC_EXPORT unsigned int GetMinScreenHeight(void);

#endif
