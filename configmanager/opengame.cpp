#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sstream>

#include "../graphiccommon/screen.h"
#define CONFIGMGR_INTERNAL
#include "opengame.h"
#include "../t5.h"

#ifdef _WIN32
#define STRDUP _strdup
#else
#include <cstring>
#define STRDUP strdup
#endif

using namespace std;

TS_Config::TS_Config(void){
	gamename = "";
    mainscript = "";
    screenwidth = 240;
    screenheight = 320;
    soundchannels = 16;
    fullscreen = false;
    systemfont = "";
    systemttffont = "";
    systemwindowstyle = "";
    systemarrow = "";
    systemuparrow = "";
    systemdownarrow = "";
    fixedplugins = 0;
    plugins = (const char **)calloc(0, 1);
}

TS_Config::~TS_Config(void){
    free(plugins);
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

void setDirectories(string basedirectory){
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
        TS_dirs->system      = "system/";
        TS_dirs->systemscript= "system/scripts";
        T5_init(2, "", TS_dirs->root);
}

void setConfig(string basedirectory){
    TS_Config *TS_conf = GetConfig();
    TS_Directories *TS_dirs = GetDirs();
	T5_file *enginefile = T5_OpenFile("engine.ini");
    TS_conf->fullscreen = (atoi(enginefile->getValueFromSection("fullscreen", "Video"))>0)?true:false;
    printf("Fixed plugins: %s\n", enginefile->getValue("fixedplugins"));
    TS_conf->fixedplugins        = atoi(enginefile->getValue("fixedplugins"));
    printf("Fixed plugins as recorded: %i\n", TS_conf->fixedplugins);
    T5_file *systemfile = T5_OpenFile(string(TS_dirs->system).append("system.ini").c_str());

    TS_conf->systemfont          = systemfile->getValue("Font");
    TS_conf->systemttffont       = systemfile->getValue("TTFFont");
    TS_conf->systemwindowstyle   = systemfile->getValue("WindowStyle");
    TS_conf->systemarrow         = systemfile->getValue("Arrow");
    TS_conf->systemuparrow       = systemfile->getValue("UpArrow");
    TS_conf->systemdownarrow     = systemfile->getValue("DownArrow");

    TS_conf->plugins             = (const char **)calloc(TS_conf->fixedplugins, sizeof(const char *));

    for(int i = 0; i<TS_conf->fixedplugins; i++){

        std::stringstream s;
        s << "plugin";
		s << i;
        TS_conf->plugins[i]=enginefile->getValue(s.str().c_str());
    }

    T5_close(enginefile);
    T5_close(systemfile);

}

void opengame(const char *Rfile)
{
	TS_Config *TS_conf = GetConfig();
	TS_Directories *TS_dirs = GetDirs();
    TS_conf->soundchannels = 32;
	printf("Opening SGM file %s\n", Rfile);
	//printf("base dir: %s\n", TS_dirs->root);
    string txt;
    std::ifstream file(Rfile, std::ios_base::in);
    if (file.is_open())
		while (file.good()){
            getline(file, txt);
			if(!txt.empty()){
				size_t nameloc=txt.find("name");
				size_t scriloc=txt.find("script");
				size_t scwiloc=txt.find("screen_width");
				size_t scheloc=txt.find("screen_height");

				if(nameloc!=string::npos&&int(nameloc)<2){
					TS_conf->gamename = STRDUP(txt.substr(nameloc+5, 128).c_str());
				}
				else if(scriloc!=string::npos&&int(scriloc)<2){
					TS_conf->mainscript = STRDUP(string(TS_dirs->script).append(txt.substr(scriloc+7, 128)).c_str());
				}
				else if(scwiloc!=string::npos&&int(scwiloc)<2){
					SetScreenWidth(atoi(txt.substr(scwiloc+13, 128).c_str()));
				}
				else if(scheloc!=string::npos&&int(scheloc)<2){
					SetScreenHeight(atoi(txt.substr(scheloc+14, 128).c_str()));
				}

			}
		}
	else {printf("game.sgm is not readable");}

    file.close();
}
