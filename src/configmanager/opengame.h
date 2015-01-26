#pragma once

// This file should remain C-clean.

#ifdef _MSC_VER
	#ifdef CONFIGMGR_INTERNAL
		#define CONFIGMGR_EXPORT __declspec(dllexport)
	#else
		#define CONFIGMGR_EXPORT __declspec(dllimport)
	#endif
#else
	#define CONFIGMGR_EXPORT extern "C"
#endif

// Why isn't system in TS_GameDirectories?
// Because TS_GameDirectories are the GAME's directories. The system directory is not dependant on the game.
struct TS_GameDirectories {
    const char *root; //!< Holds the root directory for the game, which contains the sgm file.
    const char *image; //!< The path to the default images directory.
    const char *font; //!< The path to the default rfn font directory.
    const char *script; //!< The path to the default script directory.
    const char *sound; //!< The path to the default audio file directory.
    const char *map; //!< The path to the default rmp map directory.
    const char *other; //!< The path to the other directory. There is no predefined use for this directory.
    const char *save; //!< The path to the default text and raw file directory.
    const char *spriteset; //!< The path to the default rss sprite directory.
    const char *animation; //!< The path to the default animation file directory.
    const char *windowstyle; //!< The path to the default rws directory.
    const char *soundfont; //!< The path to the default sf2 soundfont directory.
    const char *shader; //!< The path to the shader manifest folder.
};

struct TS_GameSystem {
    bool fullscreen; //!< Whether or not to load the game in fullscreen mode. This value should be updated upon changing to or from fullscreen mode.
    bool compositing; //!< Sets whether to use compositing or not. Compositing allows for cheap reading from the screen and the application of whole-scene shaders.
    int scale; //!< Graphical scale to be applied. This value does not have to be (but should be) a multiple of two. A value of 0 or a negative value result in undefined behaviour.
    const char *system;
    const char *systemscript; //!< The path to the default system script directory, used for RequireSystemScript and EvaluateSystemScript.
    const char *systemshader; //!< The path to the default system script directory, used for RequireSystemScript and EvaluateSystemScript.
    const char *systemfont; //!< The filename of the default system rfn font. This can be a path, and is relative to the system directory held in the TS_Directories given by GetDirs.
    const char *systemttffont;  //!< The filename of the default system ttf font. This can be a path, and is relative to the system directory held in the TS_Directories given by GetDirs.
    const char *systemwindowstyle;  //!< The filename of the default system rws windowstyle. This can be a path, and is relative to the system directory held in the TS_Directories given by GetDirs.
    const char *systemarrow;  //!< The filename of the default system arrow image. This can be a path, and is relative to the system directory held in the TS_Directories given by GetDirs.
    const char *systemuparrow;  //!< The filename of the default system up-arrow image. This can be a path, and is relative to the system directory held in the TS_Directories given by GetDirs.
    const char *systemdownarrow;  //!< The filename of the default system down-arrow image. This can be a path, and is relative to the system directory held in the TS_Directories given by GetDirs.
    const char *systemsoundfont;  //!< The filename of the default system sf2 soundfont. This can be a path, and is relative to the system directory held in the TS_Directories given by GetDirs.        
    const char *plugin; //!< The path to the directory that contains loaded plugins.
};

struct TS_GameConfig {
    const char *gamefunc; //!< The name of the function to be called from the start script to start the game. Historically this value has been "game". Read in from \a system.ini.
    const char *sgmname; //!< The name of the sgm file to be loaded for game configuration. Historically this value has been "game.sgm". Read in from \a system.ini.
    const char *gamename; //!< The name of the game associated with this configuration. This value is used to name the game's window, etc., and is not used to determine which game the configuration is for.
    const char *mainscript; //!< The name of the main script, which is the only script to be loaded on startup. Other scripts are loaded through function calls in this and further loaded scripts.
    unsigned screenwidth; //!< The game window width, or screen width in fullscreen.
    unsigned screenheight; //!< The game window height, or screen height in fullscreen.
    const char *author;
    const char *description;
    unsigned version; //!< Version of the Sphere API to use. So far, the only versions that are recognized are 1 and 2;
};

struct TS_GameEnvironment{
    struct TS_GameDirectories *directories;
    struct TS_GameSystem *system;
    struct TS_GameConfig *config;
};

// Intended for debugging use only.
CONFIGMGR_EXPORT void TS_PrintDirectories(struct TS_GameDirectories *that, void *c_file_handle);
CONFIGMGR_EXPORT void TS_PrintSystem(struct TS_GameSystem *that, void *c_file_handle);
CONFIGMGR_EXPORT void TS_PrintConfig(struct TS_GameConfig *that, void *c_file_handle);

// In all cases, any `const char *' passed in will be copied, and the pointer passed in must be freed(/taken care of) by the caller.
// All returned structs must be freed with 'free()'.

// Call with game's root directory.
CONFIGMGR_EXPORT struct TS_GameDirectories *TS_InitDirectories(const char *path); // Inits with the root.
CONFIGMGR_EXPORT struct TS_GameDirectories *TS_FillDirectories(struct TS_GameDirectories *that); // Assumes that->root is filled in, returns `that'.
CONFIGMGR_EXPORT void                       TS_FreeDirectories(struct TS_GameDirectories *that);

// Call with system folder and either the corresponding system.ini or system.json or nullptr for defaults.
CONFIGMGR_EXPORT struct TS_GameSystem *     TS_LoadSystem(const char *system_folder, const char *system_file); // if system_file is null, we try to load system_folder/system.json and then system_folder/system.ini
CONFIGMGR_EXPORT void                       TS_FreeSystem(struct TS_GameSystem *that);

// Call with game.sgm.
CONFIGMGR_EXPORT struct TS_GameConfig *     TS_LoadConfig(const char *path);
CONFIGMGR_EXPORT void                       TS_FreeConfig(struct TS_GameConfig *that);

CONFIGMGR_EXPORT void                       TS_AssociateContextEnvironment(void *ctx, struct TS_GameEnvironment *env);
CONFIGMGR_EXPORT struct TS_GameEnvironment *TS_GetContextEnvironment(void *ctx);
CONFIGMGR_EXPORT void                       TS_RemoveContextEnvironment(void *ctx, struct TS_GameEnvironment *env); // One of the args is NULL, the other is the one to remove any association with.
CONFIGMGR_EXPORT void                       TS_ResetContextEnvironment(void);
CONFIGMGR_EXPORT void                       TS_FreeEnvironment(struct TS_GameEnvironment *env);
