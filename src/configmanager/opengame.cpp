#include <stdlib.h>
#include <memory>
#include <cassert>
#include <string>

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

/*
template<typename T = char *, typename CT = const char *>
inline T STRDUP(CT a){
  assert(a!=nullptr);

  size_t s = strlen(a)+1;

  T r = (T)malloc(s);
  assert(r!=nullptr);

  std::copy(a, (CT)(a+s), r);
  return r;
}

*/

static v8::ArrayBuffer::Allocator* cm_static_allocator = nullptr;

void RegisterArrayBufferAllocator(v8::ArrayBuffer::Allocator* a){
    cm_static_allocator = a;
}

v8::ArrayBuffer::Allocator* RetrieveArrayBufferAllocator(void){
    return cm_static_allocator;
}

#define FIND_STRING_LAMBDA(X) [&] (const t5::map::Entry *aIter) { return (aIter)->Name == std::string(X); }

using std::string;

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

TS_Directories::TS_Directories(void)
  : MemH(nullptr){
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

    MemH = realloc(MemH, sizeof(const char *));
    ((const char **) MemH )[0] = nullptr;
}


TS_Directories::~TS_Directories(void){
  /*
    int i = 0;
    const char * lP = ((const char **)MemH)[i];

    while(lP!=nullptr){
        i++;
        free((void *)lP);
        lP = ((const char **)MemH)[i];
    }
  */
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
        TS_dirs->plugin      = "plugin/";

        /*
        int i = 0;
        const char * lP = ((const char **)TS_dirs->MemH)[i];
        while(lP!=nullptr){
            free((void *)lP);
            i++;
            lP = ((const char **)TS_dirs->MemH)[i];
        }
        */
        TS_dirs->MemH = realloc(TS_dirs->MemH, sizeof(const char *)*12);

        ((const char **)TS_dirs->MemH )[0 ] = TS_dirs->image;
        ((const char **)TS_dirs->MemH )[1 ] = TS_dirs->font;
        ((const char **)TS_dirs->MemH )[2 ] = TS_dirs->script;
        ((const char **)TS_dirs->MemH )[3 ] = TS_dirs->sound;
        ((const char **)TS_dirs->MemH )[4 ] = TS_dirs->map;
        ((const char **)TS_dirs->MemH )[5 ] = TS_dirs->other;
        ((const char **)TS_dirs->MemH )[6 ] = TS_dirs->save;
        ((const char **)TS_dirs->MemH )[7 ] = TS_dirs->spriteset;
        ((const char **)TS_dirs->MemH )[8 ] = TS_dirs->animation;
        ((const char **)TS_dirs->MemH )[9 ] = TS_dirs->windowstyle;
        ((const char **)TS_dirs->MemH )[10] = TS_dirs->soundfont;
        ((const char **)TS_dirs->MemH )[11] = TS_dirs->shader;

}

void setLocalConfig(TS_Config *c, const char *engine_conf_file, const char *system_conf_file){

    assert(engine_conf_file!=nullptr);
    assert(strlen(engine_conf_file)>0);

    assert(system_conf_file!=nullptr);
    assert(strlen(system_conf_file)>0);

    std::unique_ptr<t5::DataSource> lEngineConfSource(t5::DataSource::FromPath(t5::DataSource::eRead, engine_conf_file));
    std::unique_ptr<t5::DataSource> lSystemConfSource(t5::DataSource::FromPath(t5::DataSource::eRead, system_conf_file));
    assert(lEngineConfSource.get());
    assert(lSystemConfSource.get());

    assert(t5::IsFile(engine_conf_file) && t5::IsFile(system_conf_file));

    std::unique_ptr<t5::map::Group> lConfigGroup(new t5::map::Group("Engine", nullptr));
    std::unique_ptr<t5::map::Group> lSystemGroup(new t5::map::Group("System", nullptr));
    assert(lConfigGroup.get());
    assert(lSystemGroup.get());

    c->fullscreen = false;
    c->scale      = 1;
    c->sgmname    = "game.sgm";
    c->gamefunc   = "game";

    lConfigGroup->ReadDataSourceJSON(lEngineConfSource.get());
    lSystemGroup->ReadDataSourceJSON(lSystemConfSource.get());

    std::list<t5::map::Entry *> &lContents = lConfigGroup->Contents;

    std::list<t5::map::Entry *>::iterator lIterVideoGroup = std::find_if(lContents.begin(), lContents.end(),  FIND_STRING_LAMBDA("Video"));
    std::list<t5::map::Entry *>::iterator lIterEngineGroup = std::find_if(lContents.begin(), lContents.end(), FIND_STRING_LAMBDA("Engine"));


    if((lIterEngineGroup!=lContents.end())&&(*lIterEngineGroup)->AsGroup()){
        const char *lGamefunc = (*lIterEngineGroup)->AsGroup()->GetString("gamefunc");
        if(lGamefunc) c->gamefunc=STRDUP(lGamefunc);
        else c->gamefunc= STRDUP(c->gamefunc);

        const char *lSgmname = (*lIterEngineGroup)->AsGroup()->GetString("sgmname");
        if(lSgmname) c->sgmname=STRDUP(lSgmname);
        else c->sgmname= STRDUP(c->sgmname);
    }

    if((lIterVideoGroup!=lContents.end())&&(*lIterVideoGroup)->AsGroup()){
        c->fullscreen  = (*lIterVideoGroup)->AsGroup()->GetBool("fullscreen", false);
        c->scale       = (*lIterVideoGroup)->AsGroup()->GetInt( "scale", 1);
        c->compositing = (*lIterVideoGroup)->AsGroup()->GetInt( "compositing", 1);
    }

    //Negative scale may take on a meaning. Technically I shouldn't even cleanse these values here.
    //But I do want all graphics plugins to be able to both give and recieve something at least slightly standardized.
    if(c->scale<0){
        c->scale*=-1;
    }

    if(c->scale>16){
        c->scale = 16;
    }

    c->fixedplugins  = 0;

    c->systemfont          = STRDUP("system.rfn");
    c->systemttffont       = STRDUP("DejaVuSans.ttf");
    c->systemwindowstyle   = STRDUP("system.rws");
    c->systemarrow         = STRDUP("pointer.png");
    c->systemuparrow       = STRDUP("up_arrow.png");
    c->systemdownarrow     = STRDUP("down_arrow.png");
    c->systemsoundfont     = STRDUP("Hollywood.SF2");

    const char *lVal;

    lVal = lSystemGroup->GetString("Font");
    if(lVal) c->systemfont          = STRDUP(lVal);
    else c->systemfont= STRDUP(c->systemfont);

    lVal = lSystemGroup->GetString("TTFFont");
    if(lVal) c->systemttffont       = STRDUP(lVal);
    else c->systemttffont= STRDUP(c->systemttffont);

    lVal = lSystemGroup->GetString("WindowStyle");
    if(lVal) c->systemwindowstyle   = STRDUP(lVal);
    else c->systemwindowstyle= STRDUP(c->systemwindowstyle);

    lVal = lSystemGroup->GetString("Arrow");
    if(lVal) c->systemarrow         = STRDUP(lVal);
    else c->systemarrow = STRDUP(c->systemarrow);

    lVal = lSystemGroup->GetString("UpArrow");
    if(lVal) c->systemuparrow       = STRDUP(lVal);
    else c->systemuparrow = STRDUP(c->systemuparrow);

    lVal = lSystemGroup->GetString("DownArrow");
    if(lVal) c->systemdownarrow     = STRDUP(lVal);
    else c->systemdownarrow = STRDUP(c->systemdownarrow);

    lVal = lSystemGroup->GetString("SoundFont");
    if(lVal) c->systemsoundfont     = STRDUP(lVal);
    else c->systemsoundfont = STRDUP(c->systemsoundfont);

    c->plugins = nullptr;
}

void setConfig(const char * basedirectory){
    //GetDirs()->root;
    setLocalConfig(GetConfig());
}

int opengameLocal(const char *Rfile, TS_Config *localConf, TS_Directories *localDirs){

    std::unique_ptr<t5::DataSource> lSGMSource(t5::DataSource::FromPath(t5::DataSource::eRead, Rfile));
    assert(lSGMSource.get());

    std::unique_ptr<t5::map::Group> lSGMGroup(new t5::map::Group("SGM", nullptr));
    assert(lSGMGroup.get());

    lSGMGroup->ReadDataSourceINI(lSGMSource.get());

    localConf->soundchannels = 32;

    localConf->gamename = STRDUP(lSGMGroup->GetString("name"));

    printf("[ConfigManager] Info: Reading game configuration for %s\n", localConf->gamename);


    const char * scriptname = lSGMGroup->GetString("script");
    char * mainscriptname =  strcat((char *)calloc(strlen(localDirs->script)+strlen(scriptname)+1, 1), localDirs->script);

    mainscriptname =  strcat(mainscriptname, scriptname);

    localConf->mainscript   = (char *)calloc(strlen(localDirs->script)+strlen(scriptname)+1, 1);

    memcpy((void *)localConf->mainscript, mainscriptname, strlen(localDirs->script)+strlen(scriptname)+1);

    free((void *)mainscriptname);

    localConf->screenwidth  = lSGMGroup->GetInt("screen_width",  640);
    localConf->screenheight = lSGMGroup->GetInt("screen_height", 480);
    localConf->author       = STRDUP(lSGMGroup->GetString("author"));
    localConf->description  = STRDUP(lSGMGroup->GetString("description"));

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
