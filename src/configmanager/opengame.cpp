#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sstream>

#include <screen.h>
/*! \def CONFIGMGR_INTERNAL
Indicates that the the header files opengame.h and openscript.h are being processed from the compilation of configmanager.
*/
#define CONFIGMGR_INTERNAL
#include "opengame.h"
#include <t5.h>


/*! \file opengame.cpp
   \brief Contains functions for opening a game's configuration and metadata.

Functions are related to a games configuration, directory set, system files, main script name and the name of the game function in JavaScript and game.sgm file.
Contains the structures TS_Config and TS_Directories, which are used by plugins to get configuration, system file, and directory information for the current game.
*/

/*! \def STRDUP
   \brief STRDUP is a synonym for strdup or an equivalent or source code compatible function on the platform being used.

Redirects to strdup on Linux+GCC and _strdup on MSVC.
*/

#ifdef _MSC_VER
#define STRDUP _strdup
#else
#include <cstring>
#define STRDUP strdup
#endif

using namespace std;

TS_Config::TS_Config(void){
	gamename = "";
	gamefunc = "";
	sgmname = "";
    mainscript = "";
    screenwidth = 240;
    screenheight = 320;
    soundchannels = 16;
    fullscreen = false;
    scale = 1;
    filter = NULL;
    systemfont = "";
    systemttffont = "";
    systemwindowstyle = "";
    systemarrow = "";
    systemuparrow = "";
    systemdownarrow = "";
    fixedplugins = 0;
}

TS_Config::~TS_Config(void){
}

TS_Directories::TS_Directories(void){
	root = "";
    image = "";
    font = "";
    script = "";
    sound = "";
    map = "";
    other = "";
    save = "";
    spriteset = "";
    animation = "";
    windowstyle = "";
    system = "";
    systemscript = "";
    soundfont = "";
    systemshader = "";
    shader = "";
}


TS_Directories::~TS_Directories(void){
}

TS_Config TS_confmain;

TS_Config *GetConfig(void){
	return &TS_confmain;
}

TS_Directories TS_dirsmain;

TS_Directories *GetDirs(void){
	return &TS_dirsmain;
}

void setDirectories(const char * basedirectory){
	TS_Directories *TS_dirs = GetDirs();
	    TS_dirs->image       = STRDUP(string(TS_dirs->root).append("images/").c_str());
        TS_dirs->font        = STRDUP(string(TS_dirs->root).append("fonts/").c_str());
        TS_dirs->script      = STRDUP(string(TS_dirs->root).append("scripts/").c_str());
        TS_dirs->sound       = STRDUP(string(TS_dirs->root).append("sounds/").c_str());
        TS_dirs->map         = STRDUP(string(TS_dirs->root).append("maps/").c_str());
        TS_dirs->other       = STRDUP(string(TS_dirs->root).append("other/").c_str());
        TS_dirs->save        = STRDUP(string(TS_dirs->root).append("save/").c_str());
        TS_dirs->spriteset   = STRDUP(string(TS_dirs->root).append("spritesets/").c_str());
        TS_dirs->animation   = STRDUP(string(TS_dirs->root).append("animations/").c_str());
        TS_dirs->windowstyle = STRDUP(string(TS_dirs->root).append("windowstyles/").c_str());
        TS_dirs->soundfont   = STRDUP(string(TS_dirs->root).append("soundfonts/").c_str());
        TS_dirs->shader      = STRDUP(string(TS_dirs->root).append("shaders/").c_str());
        TS_dirs->system      = "system/";
        TS_dirs->systemscript= "system/scripts/";
        TS_dirs->systemshader= "system/shaders/";
        T5_init(2, "", TS_dirs->root);
}

void setLocalConfig(TS_Config *c){

    TS_Directories *TS_dirs = GetDirs();
	T5_file *enginefile = T5_OpenFile("engine.ini");
    c->fullscreen = (atoi(enginefile->getValueFromSection("fullscreen", "Video"))>0)?true:false;
    c->scale      = atoi(enginefile->getValueFromSection("scale", "Video"));
    c->gamefunc   = enginefile->getValueFromSection("gamefunc", "Engine");
    c->sgmname    = enginefile->getValueFromSection("sgmname", "Engine");


    //Negative scale may take on a meaning. Technically I shouldn't even cleanse these values here.
    //But I do want all graphics plugins to be able to both give and recieve something at least slightly standardized.
    if(c->scale<0){
        c->scale*=-1;
    }

    if(c->scale>16){
        c->scale = 16;
    }

    printf("[ConfigManager] Info: Fixed plugins: %s\n", enginefile->getValue("fixedplugins"));
    c->fixedplugins        = atoi(enginefile->getValue("fixedplugins"));
    printf("[ConfigManager] Info: Fixed plugins as recorded: %i\n", c->fixedplugins);
    T5_file *systemfile = T5_OpenFile(string(TS_dirs->system).append("system.ini").c_str());

    c->systemfont          = systemfile->getValue("Font");
    c->systemttffont       = systemfile->getValue("TTFFont");
    c->systemwindowstyle   = systemfile->getValue("WindowStyle");
    c->systemarrow         = systemfile->getValue("Arrow");
    c->systemuparrow       = systemfile->getValue("UpArrow");
    c->systemdownarrow     = systemfile->getValue("DownArrow");
    c->systemsoundfont     = systemfile->getValue("SoundFont");

    c->plugins             = (const char **)calloc(c->fixedplugins, sizeof(const char *));

    for(int i = 0; i<c->fixedplugins; i++){

        std::stringstream s;
        s << "plugin";
		s << i;
        c->plugins[i]=enginefile->getValue(s.str().c_str());
    }

    T5_close(enginefile);
    T5_close(systemfile);
}

void setConfig(const char * basedirectory){
    TS_Config *TS_conf = GetConfig();
    TS_Directories *TS_dirs = GetDirs();
	T5_file *enginefile = T5_OpenFile("engine.ini");
    TS_conf->fullscreen = (atoi(enginefile->getValueFromSection("fullscreen", "Video"))>0)?true:false;
    TS_conf->compositing= (atoi(enginefile->getValueFromSection("compositing", "Video"))>0)?true:false;
    TS_conf->scale      = atoi(enginefile->getValueFromSection("scale", "Video"));
    if(enginefile->getValueFromSection("gamefunc", "Engine")==NULL)
        TS_conf->gamefunc = STRDUP("game");
    else
        TS_conf->gamefunc   = STRDUP(enginefile->getValueFromSection("gamefunc", "Engine"));
    if(enginefile->getValueFromSection("sgmname", "Engine")==NULL)
        TS_conf->sgmname = STRDUP("game.sgm");
    else
        TS_conf->sgmname    = STRDUP(enginefile->getValueFromSection("sgmname", "Engine"));



    //Negative scale may take on a meaning. Technically I shouldn't even cleanse these values here.
    //But I do want all graphics plugins to be able to both give and recieve something at least slightly standardized.
    if(TS_conf->scale<0){
        TS_conf->scale*=-1;
    }

    if(TS_conf->scale>16){
        TS_conf->scale = 16;
    }

    printf("[ConfigManager] Info: Fixed plugins: %s\n", enginefile->getValue("fixedplugins"));
    TS_conf->fixedplugins        = atoi(enginefile->getValue("fixedplugins"));
    printf("[ConfigManager] Info: Fixed plugins as recorded: %i\n", TS_conf->fixedplugins);
    T5_file *systemfile = T5_OpenFile(string(TS_dirs->system).append("system.ini").c_str());

    TS_conf->systemfont          = STRDUP(systemfile->getValue("Font"));
    TS_conf->systemttffont       = STRDUP(systemfile->getValue("TTFFont"));
    TS_conf->systemwindowstyle   = STRDUP(systemfile->getValue("WindowStyle"));
    TS_conf->systemarrow         = STRDUP(systemfile->getValue("Arrow"));
    TS_conf->systemuparrow       = STRDUP(systemfile->getValue("UpArrow"));
    TS_conf->systemdownarrow     = STRDUP(systemfile->getValue("DownArrow"));
    TS_conf->systemsoundfont     = STRDUP(systemfile->getValue("SoundFont"));

    //TS_conf->plugins             = (const char **)calloc(TS_conf->fixedplugins, sizeof(const char *));
/*
    for(int i = 0; i<TS_conf->fixedplugins; i++){

        std::stringstream s;
        s << "plugin";
		s << i;
        TS_conf->plugins[i]=enginefile->getValue(s.str().c_str());
    }
*/
    T5_close(enginefile);
    T5_close(systemfile);

}

int opengameLocal(const char *Rfile, TS_Config *localConf, TS_Directories *localDirs){

    localConf->soundchannels = 32;
	printf("[ConfigManager] Info: Opening SGM file %s\n", Rfile);
	T5_file *file = T5_OpenFile(Rfile);

    if(file==NULL){
        return 1;
    }

	localConf->gamename     = STRDUP(file->getValue("name"));
	printf("[ConfigManager] Info: Reading game configuration for %s\n", localConf->gamename);


	const char *scriptname  = file->getValue("script");
	char * mainscriptname =  strcat((char *)calloc(strlen(localDirs->script)+strlen(scriptname)+1, 1), localDirs->script);

	mainscriptname =  strcat(mainscriptname, scriptname);

    localConf->mainscript   = (char *)calloc(strlen(localDirs->script)+strlen(scriptname)+1, 1);

    memcpy((void *)localConf->mainscript, mainscriptname, strlen(localDirs->script)+strlen(scriptname)+1);

    free((void *)mainscriptname);

	localConf->screenwidth  = atoi(file->getValue("screen_width"));
	localConf->screenheight = atoi(file->getValue("screen_height"));
	localConf->author       = STRDUP(file->getValue("author"));
	localConf->description   = STRDUP(file->getValue("description"));

     T5_close(file);

    return 0;

}

void opengame(const char *Rfile){

    int err = opengameLocal(Rfile, GetConfig(), GetDirs());
    if(err){
	    printf("[ConfigManager] Error: game.sgm is not readable");
        return;
    }

    SetScreenHeight(GetConfig()->screenheight);
    SetScreenWidth(GetConfig()->screenwidth);

    return;
}
