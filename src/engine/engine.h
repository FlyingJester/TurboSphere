#ifndef ENGINE_HEAD
#define ENGINE_HEAD
//extern v8::Persistent<v8::Context> context;

#define V8FLAGS "--harmony_typed_arrays"

#include <string>
#include <string.h>
#include <math.h>
#define V8_ALLOW_ACCESS_TO_RAW_HANDLE_CONSTRUCTOR
#define V8_USE_UNSAFE_HANDLES

#include <t5.h>
#include <v8.h>
#if defined _WIN32
/*
    #include "SDL2/SDL.h"
    #include "SDL2/SDL_ttf.h"
    #include "SDL2/SDL_image.h"
    #include "SDL2/SDL_thread.h"
*/

    //File handling generalization.
    #include <windows.h>

    typedef HANDLE filehandle;
    typedef WIN32_FIND_DATA filedata;
    #define FILENAME(NAME) NAME.cFileName
    #define ISDIRECTORY(NAME) (NAME.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
#endif

#ifndef _WIN32

/*
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_thread.h>
*/
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

#include <opengame.h>

struct TS_ConfigOverride {
public:
	TS_ConfigOverride();
	~TS_ConfigOverride();
	bool gamefunc;
	bool sgmname;
    bool gamename;
    bool mainscript;
    bool screenwidth;
    bool screenheight;
    bool soundchannels;
    bool fullscreen;
    bool compositing;
    bool scale;
    bool systemfont;
    bool systemttffont;
    bool systemwindowstyle;
    bool systemarrow;
    bool systemuparrow;
    bool systemdownarrow;
    bool systemsoundfont;
    bool author;
	bool description;
	bool fixedplugins;
    bool plugins;
    TS_Config *config;
};

void runGame(const char * path, TS_ConfigOverride *overrideConf = NULL);

#endif
