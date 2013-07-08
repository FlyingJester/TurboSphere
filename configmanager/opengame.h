#ifndef OPENGAME_HEAD
#define OPENGAME_HEAD

#include <string>
#ifdef _WIN32
	#ifdef CONFIGMGR_INTERNAL
		#define CONFIGMGR_EXPORT __declspec(dllexport)
	#else
		#define CONFIGMGR_EXPORT __declspec(dllimport)
	#endif
#define CONFIGMGRCALL _cdecl
#else
	#define CONFIGMGR_EXPORT extern "C"
    #define CONFIGMGRCALL  //
#endif

using namespace std;


#ifdef _MSC_VER
typedef struct CONFIGMGR_EXPORT TS_Directories {
#else
struct TS_Directories {
#endif
public:
	TS_Directories(void);
	~TS_Directories(void);
	const char * root;
    const char * image;
    const char * font;
    const char * script;
    const char * sound;
    const char * map;
    const char * other;
    const char * save;
    const char * spriteset;
    const char * animation;
    const char * windowstyle;
    const char * soundfont;
    const char * system;
    const char * systemscript;
#ifdef _MSC_VER
} TS_Directories;
#else
};
#endif

#ifdef _MSC_VER
struct CONFIGMGR_EXPORT TS_Config {
#else
struct TS_Config {
#endif
public:
	TS_Config();
	~TS_Config();
    const char * gamename;
    const char * mainscript;
    int screenwidth;
    int screenheight;
    int soundchannels;
    bool fullscreen;
    int scale;
    void *filter;
    const char *systemfont;
    const char *systemttffont;
    const char *systemwindowstyle;
    const char *systemarrow;
    const char *systemuparrow;
    const char *systemdownarrow;
    const char *systemsoundfont;
    int fixedplugins;
    const char ** plugins;
    void *reserved;
#ifdef _MSC_VER
} TS_Config;
#else
};
#endif

CONFIGMGR_EXPORT TS_Config *GetConfig(void);
CONFIGMGR_EXPORT TS_Directories *GetDirs(void);
CONFIGMGR_EXPORT void opengame(const char *Rfile);
CONFIGMGR_EXPORT void setDirectories(string basedir);
CONFIGMGR_EXPORT void setConfig(string);

#endif
