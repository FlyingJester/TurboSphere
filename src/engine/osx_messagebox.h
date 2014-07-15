#ifndef OSX_MESSAGEBOX_HEAD
#define OSX_MESSAGEBOX_HEAD

/////
// Simple messagebox from OS X, wrapped for C/C++ usage.
//
// Designed for use by Abort and Error, particularly useful
// since we don't want to go poking the system SDL2 framework
// from the engine proper.
//

#ifdef __cplusplus
    #define OBJCEXTERN extern "C"
    #define OBJCDEFARGS(x) = x
#else
#define OBJCEXTERN
#define OBJCDEFARGS(x)
#endif

OBJCEXTERN void TS_OSX_MessageBox(const char * title, const char *content);

#endif
