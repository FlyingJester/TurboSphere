#ifndef TS_COMMON_DLOPENWRAP
#define TS_COMMON_DLOPENWRAP


#ifdef _WIN32
#include <windows.h>
typedef HINSTANCE fhandle;
#else
typedef void* fhandle;
#endif

#ifdef _WIN32
#include <windows.h>
#define DLOPENFUNCTION(_type, _dlfunc, _handle, _name, _errormsg, _error, action)\
    _dlfunc = (_type)GetProcAddress(_handle, _name);\
    if ((_dlfunc) == NULL)  {\
        printf("Error loading library : No function \"%s\".\n", _name);\
        action;\
    }

#else

#include <stdlib.h>
#include <dlfcn.h>

#define DLOPENFUNCTION(_type, _dlfunc, _handle, _name, _errormsg, _error, action)\
    _dlfunc = (_type)dlsym(_handle, _name);\
    if ((_error = dlerror()) != NULL)  {\
        fprintf (stderr, "%s\n", error);\
        printf(_errormsg, _name);\
        action;\
    }

#endif

#endif
