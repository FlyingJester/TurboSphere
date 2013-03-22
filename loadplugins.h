#ifndef LOADPLUGINS_HEAD
#define LOADPLUGINS_HEAD
typedef const char * pluginName;

int loadAllPlugins();

#ifdef _WIN32
#include <windows.h>
typedef HINSTANCE fhandle;
#else
typedef void* fhandle;
#endif

void grabFuncsFromLibrary(fhandle handle);

#endif
