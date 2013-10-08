#ifndef OPENGAME_HEAD
#define OPENGAME_HEAD

/*! \file opengame.h
    \brief Contains functions for opening a game's configuration and metadata.
    Functions are related to a games configuration, directory set, system files, main script name and the name of the game function in JavaScript and game.sgm file.
*/

/*! \def CONFIGMGR_EXPORT
Defines the shared library export code for the platform when CONFIGMGR_INTERNAL is undefined, and the shared library import code for the platform otherwise.
*/

/*! \def CONFIGMGRCALL
Defines exported functions' calling convention on the platform.
*/

#include <string>
#ifdef _MSC_VER
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


/*! \struct TS_Directories
   \brief Holds the local and system directories for a game.

All values are read in from system.ini unless otherwise noted.
*/
#ifdef _MSC_VER
typedef struct CONFIGMGR_EXPORT TS_Directories {
#else
struct TS_Directories {
#endif
public:
	TS_Directories(void);
	~TS_Directories(void);
	const char * root; //!< Holds the root directory for the game, which contains the sgm file.
    const char * image; //!< The path to the default images directory.
    const char * font; //!< The path to the default rfn font directory.
    const char * script; //!< The path to the default script directory.
    const char * sound; //!< The path to the default audio file directory.
    const char * map; //!< The path to the default rmp map directory.
    const char * other; //!< The path to the other directory. There is no predefined use for this directory.
    const char * save; //!< The path to the default text and raw file directory.
    const char * spriteset; //!< The path to the default rss sprite directory.
    const char * animation; //!< The path to the default animation file directory.
    const char * windowstyle; //!< The path to the default rws directory.
    const char * soundfont; //!< The path to the default sf2 soundfont directory.
    const char * system; //!< The path to the system directory.
    const char * systemscript; //!< The path to the default system script directory, used for RequireSystemScript and EvaluateSystemScript.
#ifdef _MSC_VER
} TS_Directories;
#else
};
#endif


/*! \fn GetDirs
*   \brief Returns a pointer to a TS_Directories struct that holds the current game's directory paths.
*
* The structure returned is modified by values in the engine.ini file.
*/
CONFIGMGR_EXPORT TS_Directories *GetDirs(void);

/*! \struct TS_Config
*   \brief Holds game configuration and system files.
*
* All values are read in from engine.ini unless otherwise noted.
*/
#ifdef _MSC_VER
struct CONFIGMGR_EXPORT TS_Config {
#else
struct TS_Config {
#endif
public:
	TS_Config();
	~TS_Config();
	const char * gamefunc; //!< The name of the function to be called from the start script to start the game. Historically this value has been "game". Read in from \a system.ini.
	const char * sgmname; //!< The name of the sgm file to be loaded for game configuration. Historically this value has been "game.sgm". Read in from \a system.ini.
    const char * gamename; //!< The name of the game associated with this configuration. This value is used to name the game's window, etc., and is not used to determine which game the configuration is for.
    const char * mainscript; //!< The name of the main script, which is the only script to be loaded on startup. Other scripts are loaded through function calls in this and further loaded scripts.
    int screenwidth; //!< The game window width, or screen width in fullscreen.
    int screenheight; //!< The game window height, or screen height in fullscreen.
    int soundchannels; //!< The number of preloaded sound channels on startup. This value is deprecated for the audioBass sound plugin.
    bool fullscreen; //!< Whether or not to load the game in fullscreen mode. This value should be updated upon changing to or from fullscreen mode.
    int scale; //!< Graphical scale to be applied. This value does not have to be (but should be) a multiple of two. A value of 0 or a negative value result in undefined behaviour.
    void *filter; //!< A pointer to a filter to be applied to the video output. Not implemented yet.
    const char *systemfont; //!< The filename of the default system rfn font. This can be a path, and is relative to the system directory held in the TS_Directories given by GetDirs.
    const char *systemttffont;  //!< The filename of the default system ttf font. This can be a path, and is relative to the system directory held in the TS_Directories given by GetDirs.
    const char *systemwindowstyle;  //!< The filename of the default system rws windowstyle. This can be a path, and is relative to the system directory held in the TS_Directories given by GetDirs.
    const char *systemarrow;  //!< The filename of the default system arrow image. This can be a path, and is relative to the system directory held in the TS_Directories given by GetDirs.
    const char *systemuparrow;  //!< The filename of the default system up-arrow image. This can be a path, and is relative to the system directory held in the TS_Directories given by GetDirs.
    const char *systemdownarrow;  //!< The filename of the default system down-arrow image. This can be a path, and is relative to the system directory held in the TS_Directories given by GetDirs.
    const char *systemsoundfont;  //!< The filename of the default system sf2 soundfont. This can be a path, and is relative to the system directory held in the TS_Directories given by GetDirs.
    int fixedplugins; //!< Deprecated
    const char ** plugins; //!< For private use by the engine.
    void *reserved; //!< For private use by the engine.
#ifdef _MSC_VER
};
#else
};
#endif

/*!Returns a pointer to a TS_Config struct that holds the current game's configuration.
*
*/
CONFIGMGR_EXPORT TS_Config *GetConfig(void);


/*!Reads data from a specified game.sgm file and sets the current configuration based on those values.
*
*/
CONFIGMGR_EXPORT void opengame(const char *rfile /**<The name or path of the sgm file to read into the current TS_Config structure as given by \link GetConfig \endlink. Path is relative to the root directory in the current TS_Directories as given by \link GetDirs \endlink.*/);

/*!Sets the current game's directory paths to the default paths using a given game root directory.
\param basedir The directory to use as the base for the other directories in the configuration.
*/
CONFIGMGR_EXPORT void setDirectories(const char * basedir);

/*!Sets the current game's configuration to the one specified by the config file in the given game root directory.
\param basedir The directory containing the game.sgm and system.ini files to read.
*/
CONFIGMGR_EXPORT void setConfig(const char * basedir);

#endif
