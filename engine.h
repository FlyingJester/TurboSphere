#ifndef ENGINE_HEAD
#define ENGINE_HEAD
//extern v8::Persistent<v8::Context> context;

#include <string>
#include <math.h>
#define V8_ALLOW_ACCESS_TO_RAW_HANDLE_CONSTRUCTOR
#define V8_USE_UNSAFE_HANDLES

#include "t5.h"
#if defined _WIN32
    #include "v8.h"
    #include "SDL/SDL.h"
    #include "SDL/SDL_ttf.h"
    #include "SDL/SDL_image.h"
    #include "SDL/SDL_thread.h"


    //File handling generalization.
    #include <windows.h>

    typedef HANDLE filehandle;
    typedef WIN32_FIND_DATA filedata;
    #define FILENAME(NAME) NAME.cFileName
    #define ISDIRECTORY(NAME) (NAME.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
#endif

#ifndef _WIN32

#include "v8.h"
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_thread.h>

    //File handling generalization.
    #include <dirent.h>

    typedef DIR * filehandle;
    typedef struct dirent * filedata;
    #define FILENAME(NAME) NAME->d_name
    #define ISDIRECTORY(NAME) (NAME->d_type==DT_DIR)

#endif

#undef V8_ALLOW_ACCESS_TO_RAW_HANDLE_CONSTRUCTOR
#undef V8_USE_UNSAFE_HANDLES


#if defined _WIN32

#else

#endif


#endif
