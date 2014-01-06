//#include "SDL_V8.h"
#include "engine.h"
#include "configmanager/openscript.h"
#include "configmanager/opengame.h"
#include <cstring>
#include <cctype>
#include "functionload.h"
#include "variableregister.h"
#include "loadplugins.h"
#include "typed_array.h"

#include "common/noreturn.h"


#include "t5.h"

#ifdef __linux__
#define ENGINE "[Engine]"
#else
#define ENGINE "[Engine]"
#endif

#include <ctime>

void (*TS_MessageBox)(const char * title, const char *content);
int (*SDL_ShowSimpleMessageBox)(uint32_t flags, const char *title, const char *message, void *window);
void (*TS_Sleep)(uint32_t milliseconds);
uint32_t (*TS_GetTime)(void);

#ifdef _WIN32

#include <mmsystem.h>

//Get Sleep function
#if NTDDI_VERSION == NTDDI_WIN8
    #include <Synchapi.h>
#else
    #include <WinBase.h>
#endif

#define STRDUP _strdup
#define snprintf _snprintf
#if _MSC_VER <= 1600
	#define isblank(x) ((x==0x9)||(x==0x20))
	#define isprint(x) ((x>=0x20)&&(x<0x7F))
#endif
#else

#ifdef HAS_UNISTD_SYS
#include <sys/unistd.h>
    #define TS_SLEEP(x) usleep(x*1000)
#elif defined HAS_UNISTD
#include <unistd.h>
    #define TS_SLEEP(x) usleep(x*1000)
#endif

#include <sys/time.h>
#define STRDUP strdup
#endif

//////////////////////////////////////////////////////////////
//Always update!
//////////////////////////////////////////////////////////////
#define VERSION "0.3.5e"

inline void TS_OverrideConfig(TS_Config *conf, TS_ConfigOverride *overrideConf){
    TS_Config *oconf = overrideConf->config;
    if(overrideConf->gamefunc)
        conf->gamefunc = oconf->gamefunc;

	if(overrideConf->sgmname)
        conf->sgmname = oconf->sgmname;

	if(overrideConf->gamename)
        conf->gamename = oconf->gamename;

	if(overrideConf->mainscript)
        conf->mainscript = oconf->mainscript;

	if(overrideConf->screenwidth)
        conf->screenwidth = oconf->screenwidth;

	if(overrideConf->mainscript)
        conf->screenheight = oconf->screenheight;

	if(overrideConf->fullscreen)
        conf->fullscreen = oconf->fullscreen;

	if(overrideConf->compositing)
        conf->compositing = oconf->compositing;

	if(overrideConf->scale)
        conf->scale = oconf->scale;

	if(overrideConf->systemfont)
        conf->systemfont = oconf->systemfont;

	if(overrideConf->systemttffont)
        conf->systemttffont = oconf->systemttffont;

	if(overrideConf->systemwindowstyle)
        conf->systemwindowstyle = oconf->systemwindowstyle;

	if(overrideConf->systemarrow)
        conf->systemarrow = oconf->systemarrow;

	if(overrideConf->systemuparrow)
        conf->systemuparrow = oconf->systemuparrow;

	if(overrideConf->systemsoundfont)
        conf->systemsoundfont = oconf->systemsoundfont;

	if(overrideConf->fixedplugins)
        conf->fixedplugins = oconf->fixedplugins;

	if(overrideConf->plugins)
        conf->plugins = oconf->plugins;

}


TS_ConfigOverride::TS_ConfigOverride(){
	gamefunc = false;
	sgmname = false;
    gamename = false;
    mainscript = false;
    screenwidth = false;
    screenheight = false;
    soundchannels = false;
    fullscreen = false;
    compositing = false;
    scale = false;
    systemfont = false;
    systemttffont = false;
    systemwindowstyle = false;
    systemarrow = false;
    systemuparrow = false;
    systemdownarrow = false;
    systemsoundfont = false;
    author = false;
	description = false;
	fixedplugins = false;
    plugins = false;
    config = NULL;
}

void TS_SDLMessageBox(const char *title, const char *content){
    SDL_ShowSimpleMessageBox(0x00000040, title, content, NULL);
}

void TS_TerminalMessageBox(const char *title, const char *content){
    printf(ENGINE " Info: %s\n", title);
    printf("\t%s\n", content);
}

void * SDLhandle = NULL;

void LoadMessageBoxFunctions(void){
    SDLhandle = dlopen("libSDL2.so", RTLD_GLOBAL|RTLD_NOW);
    if(SDLhandle!=NULL){
        SDL_ShowSimpleMessageBox = (int(*)(uint32_t, const char *, const char *, void *))dlsym(SDLhandle, "SDL_ShowSimpleMessageBox");
        TS_MessageBox = TS_SDLMessageBox;
        TS_Sleep = (void(*)(uint32_t))dlsym(SDLhandle, "SDL_Delay");
        TS_GetTime = (uint32_t(*)(void))dlsym(SDLhandle, "SDL_GetTicks");
    }

    if(SDL_ShowSimpleMessageBox==NULL){
        printf(ENGINE " Warning: No suitable MessageBox backends. Defaulting to terminal output.\n");
        TS_MessageBox = TS_TerminalMessageBox;
    }


}

void CloseMessageBoxFunctions(void){
    if(SDLhandle==NULL){
        dlclose(SDLhandle);
        SDLhandle = NULL;
        TS_Sleep = NULL;
        SDL_ShowSimpleMessageBox=NULL;
    }
    TS_MessageBox=NULL;
}

static char ** loadedScripts = NULL;
static int numUniqueScriptsLoaded = 0;

void TS_FreeLoadedScripts(void){
    for(int i = 0; i<numUniqueScriptsLoaded; i++){
        free((void *)loadedScripts[i]);
        loadedScripts[i] = NULL;
    }
    free(loadedScripts);
    loadedScripts = NULL;
    numUniqueScriptsLoaded = 0;
}

void exitContext(v8::Persistent<v8::Context> context){
    context->v8::Context::Exit();
}

void TS_CallFunc(const char *name, v8::Persistent<v8::Context> context){
        v8::Handle<v8::Value> gameValue = context->Global()->Get(v8::String::New(name));
        if(false){
            printf(ENGINE "TS_CallGame error: game is not defined.");
            return;
        }
        if(!gameValue->IsFunction()){
            printf(ENGINE "TS_CallGame error: game is not a function.");
            return;
        }

        v8::Handle<v8::Function> gameFunc = v8::Handle<v8::Function>::Cast(gameValue);
        gameFunc->Call(context->Global(), 0, NULL);
}


v8::Handle<v8::Value> GetVersionNumber(const v8::Arguments& args){
    return v8::Number::New(2.0);
}

v8::Handle<v8::Value> GetVersionString(const v8::Arguments& args){
    return v8::String::New(VERSION);
}

v8::Handle<v8::Value> GarbageCollect(const v8::Arguments& args){
    int collected = 0;
    while(!v8::V8::IdleNotification(1000)){
            if(collected==1)
                printf(ENGINE " Info: Garbage Collecting ");
            if(collected>1)
                printf(".");
            collected++;
            v8::V8::LowMemoryNotification();
    }
    v8::V8::LowMemoryNotification();
    if(collected>1)
        printf("\n");
    return v8::Undefined();
}

TS_NORETURN v8::Handle<v8::Value> ExitGame(const v8::Arguments& args){
    exit(0);
}

TS_NORETURN v8::Handle<v8::Value> AbortGame(const v8::Arguments& args){
    if(args.Length()<1){
        ExitGame(args);
    }
    v8::String::AsciiValue str(args[0]);
    TS_MessageBox(GetConfig()->gamename, *str);
    ExitGame(args);
}

v8::Handle<v8::Value> RequireScript(const v8::Arguments& args){
    if(args.Length()<1){
        return v8::ThrowException(v8::Exception::Error(v8::String::New(ENGINE " RequireScript Error: Called with no arguments.")));
    }
    if(!args[0]->IsString()) {
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New(ENGINE " RequireScript Error: Argument 0 is not a string.")));
    }
    v8::String::AsciiValue str(args[0]);
    const char *scriptname = *str;

    for(int i = 0; i< numUniqueScriptsLoaded; i++){
        if(strcmp(scriptname, loadedScripts[i])==0){
            printf(ENGINE " RequireScript Info: Did not load script %s, same as script number %i.\n", scriptname, i);

            return v8::Undefined();
        }
    }

    numUniqueScriptsLoaded++;
    loadedScripts = (char **)realloc(loadedScripts, numUniqueScriptsLoaded*(sizeof(char *)));

    loadedScripts[numUniqueScriptsLoaded-1] = STRDUP(scriptname);

    //printf("[Engine] RequireScript Info: Loaded script %s as script number %i.\n", loadedScripts[numUniqueScriptsLoaded-1], numUniqueScriptsLoaded-1);

    return TS_LoadScript(args);
}

v8::Handle<v8::Value> GetTime(const v8::Arguments& args)
{

    #ifdef _WIN32

	return  v8::Number::New(timeGetTime());

    #else
    struct timespec s;

    clock_gettime(CLOCK_MONOTONIC, &s);

	return  v8::Number::New((s.tv_sec*1000)+(s.tv_nsec/1000000));//>>20)); //1000000

    //return v8::Number::New(TS_GetTime());
    #endif
}

v8::Handle<v8::Value> Delay(const v8::Arguments& args){
	unsigned int t = args[0]->Uint32Value();

	#ifdef _WIN32

	#else
	//If we are delaying for longer than the longest time it ever took to call v8::V8::LowMemoryNotification(),
	//Then we will call it and subtract the time difference it made from the ms to sleep for.
	static uint32_t max = 0;
	if(t>max){

        struct timespec s;

        clock_gettime(CLOCK_MONOTONIC, &s);
        uint32_t ot = (s.tv_sec*1000)+(s.tv_nsec/1000000);

        while(!v8::V8::IdleNotification(1000)){}

        v8::V8::LowMemoryNotification();


        clock_gettime(CLOCK_MONOTONIC, &s);
        ot = (s.tv_sec*1000)+(s.tv_nsec/1000000)-ot;
        t-=ot;
        if(ot>max){
            max=ot;
            printf(ENGINE " Info: Increased minimum time threshold to %i.\n", max);
        }
	}

        TS_SLEEP(t);
	#endif  //_WIN32

	//SDL_Delay(t);
	return v8::Undefined();
}

void notationCallback(v8::GCType type, v8::GCCallbackFlags flags){
    printf(ENGINE " GC was performed.\n");
}

inline char * destructiveTrim(char * c){

    while(*c!='\0'){
        if((!isprint(*c))||(isblank(*c))){
            c++;
        }
        else{
            break;
        }
    }

    return c;
}

#define LINE_NUM_CHAR_SIZE 6
#define LINE_NUM_FILL 32, 32, 32, 32, 32, 32, 0

void TS_MessageCallback(v8::Handle<v8::Message> message, v8::Handle<v8::Value> data){
    char LineNumCharl[LINE_NUM_CHAR_SIZE+1] = {LINE_NUM_FILL};
    char *LineNumChar = LineNumCharl;
    fprintf(stderr, "Fatal Error\n");

    //auto CurrentTrace =  v8::StackTrace::CurrentStackTrace(0x0FFFFFFF);
    //auto CurrentFrame = CurrentTrace->GetFrame(CurrentTrace->GetFrameCount()-1);
    //v8::String::Utf8Value FuncName(CurrentFrame->GetFunctionName());
    //v8::String::Utf8Value ScriptName(CurrentFrame->GetScriptName());
    v8::String::Utf8Value LineContents(message->GetSourceLine());

    v8::String::Utf8Value Reason(message->Get());

    int LineNum = message->GetLineNumber();

    v8::String::Utf8Value ScriptName(message->GetScriptResourceName());
    const char * Name = "TurboSphere encountered a fatal exception in script.";
    std::string str;

    str+= *Reason;
    str+= "\n";

    str+= "Error occured in script ";
    str+= *ScriptName;
    str+= " at line ";
    snprintf(LineNumCharl, LINE_NUM_CHAR_SIZE, "%6i", LineNum);
    char *LineContentsl = destructiveTrim(*LineContents);
    LineNumChar = destructiveTrim(LineNumChar);

    str+= LineNumChar;
    str+= ":\n";
    str+= LineContentsl;

    fprintf(stderr, str.c_str());
    fprintf(stderr, "\n");
    TS_MessageBox(Name, str.c_str());

}

void runGame(const char * path, TS_ConfigOverride *overrideConf){


    v8::V8::SetFlagsFromString(V8FLAGS, strlen(V8FLAGS));

    v8::V8::SetArrayBufferAllocator(new TS_ArrayBufferAllocator());

    char * dir;

    const char *gameSGMfile;

    TS_Config *TS_conf = GetConfig();
    TS_Directories *TS_dirs = GetDirs();

    dir = STRDUP(path);
    size_t path_size_chr = strlen(dir);

    if(T5_IsDir(path)){

        if(dir[path_size_chr-1]=='/'){
            TS_dirs->root = STRDUP(dir);
            setDirectories(TS_dirs->root);
            setConfig(TS_dirs->root);
            printf(ENGINE " Info: %s is the sgmname. We are running in / mode.\n", TS_conf->sgmname);
            gameSGMfile = STRDUP(string(dir).append(TS_conf->sgmname).c_str());
        }
        else {
            TS_dirs->root = STRDUP(string(dir).append("/").c_str());
            setDirectories(TS_dirs->root);
            setConfig(TS_dirs->root);
            printf(ENGINE " Info: %s is the sgmname. We are running in NO / mode.\n", TS_conf->sgmname);
            gameSGMfile = STRDUP((string(TS_dirs->root)).append(TS_conf->sgmname).c_str());
        }
    }
    else if (T5_IsFile(path)){

        //If the path ends in a slash, we need to remove it so that we can use just the filename.

        //This stops crashes with the QT+T5 launcher when it is set to use directories, and then
        //that setting is overridden by check in the open... dialog.
        if(dir[path_size_chr]=='/'){
            dir[path_size_chr]='\0';
        }

        //Before we splice out the the last part of the path, use it as the file.
        gameSGMfile = STRDUP(dir);
        //Since it is a file, we cut off the filename and turn dir into just the directory.
        memset(strrchr((char *)dir, '/'), '\0', 1);


        TS_dirs->root = STRDUP(string(dir).append("/").c_str());
        setDirectories(TS_dirs->root);
        setConfig(TS_dirs->root);
    }
    else{
        if(stderr==stdin)
            fprintf(stderr, ENGINE " Error: Invalid path '%s' given for the game.\n", path);
        else
            fprintf(stderr, "[Engine] Error: Invalid path '%s' given for the game.\n", path);

        free((void *)dir);
        exit(0);
    }

    if((overrideConf!=NULL)&&(overrideConf->config!=NULL)){
        TS_OverrideConfig(TS_conf, overrideConf);
    }

    LoadMessageBoxFunctions();

    v8::HandleScope mainscope(v8::Isolate::GetCurrent());

    opengame(gameSGMfile);
    free((void *)gameSGMfile);

    setDirectories(TS_dirs->root);

    loadAllPlugins();

    printf(ENGINE " Info: All Plugins initialized.\n");
    printf(ENGINE " Info: The main script is %s\n", TS_conf->mainscript);

	std::string ScriptFileText = openfile(TS_conf->mainscript);

	const char *script_name = TS_conf->mainscript;

    v8::Persistent<v8::Context> context(v8::Isolate::GetCurrent(), v8::Context::New(v8::Isolate::GetCurrent()));


    context->v8::Context::Enter();

	v8::Handle<v8::FunctionTemplate> E_EvaluateScripttempl = v8::FunctionTemplate::New(TS_LoadScript);

	v8::Handle<v8::FunctionTemplate> E_RequireScripttempl = v8::FunctionTemplate::New(RequireScript);

	v8::Handle<v8::FunctionTemplate> E_RequireSystemScripttempl = v8::FunctionTemplate::New(TS_LoadSystemScript);

	v8::Handle<v8::FunctionTemplate> E_GetVersionNumbertempl = v8::FunctionTemplate::New(GetVersionNumber);

	v8::Handle<v8::FunctionTemplate> E_GetVersionStringtempl = v8::FunctionTemplate::New(GetVersionString);

	v8::Handle<v8::FunctionTemplate> E_GetTimetempl = v8::FunctionTemplate::New(GetTime);

	v8::Handle<v8::FunctionTemplate> E_Delaytempl = v8::FunctionTemplate::New(Delay);

	v8::Handle<v8::FunctionTemplate> E_Exittempl = v8::FunctionTemplate::New(ExitGame);

	v8::Handle<v8::FunctionTemplate> E_Aborttempl = v8::FunctionTemplate::New(AbortGame);

	v8::Handle<v8::FunctionTemplate> E_GarbageCollecttempl = v8::FunctionTemplate::New(GarbageCollect);


    v8::Context::Scope context_scope(context);

    registerAllFunctions(context);
    registerAllVariables(context);

	context->Global()->Set(v8::String::New("EvaulateScript"), E_EvaluateScripttempl->GetFunction());

	context->Global()->Set(v8::String::New("RequireScript"), E_RequireScripttempl->GetFunction());

	context->Global()->Set(v8::String::New("RequireSystemScript"), E_RequireSystemScripttempl->GetFunction());

	context->Global()->Set(v8::String::New("GetVersion"), E_GetVersionNumbertempl->GetFunction());

	context->Global()->Set(v8::String::New("GetVersionString"), E_GetVersionStringtempl->GetFunction());

	context->Global()->Set(v8::String::New("GetTime"), E_GetTimetempl->GetFunction());

	context->Global()->Set(v8::String::New("Delay"), E_Delaytempl->GetFunction());

	context->Global()->Set(v8::String::New("Exit"), E_Exittempl->GetFunction());

	context->Global()->Set(v8::String::New("Abort"), E_Aborttempl->GetFunction());

	context->Global()->Set(v8::String::New("GarbageCollect"), E_GarbageCollecttempl->GetFunction());

    //v8::V8::SetCaptureStackTraceForUncaughtExceptions(true);

    v8::V8::AddMessageListener(TS_MessageCallback);

	ExecuteString(v8::String::New(ScriptFileText.c_str()), v8::String::New(script_name), true);
	printf(ENGINE " Info: Running Script.\n");

    printf(ENGINE " Info: Optimizing JS Stack before calling Game Function ");
    while(!v8::V8::IdleNotification(1000)){printf("."); v8::V8::LowMemoryNotification();}
    printf("\n");

    TS_CallFunc("game", context);

	printf(ENGINE " Info: Game function done running.\n");

    v8::V8::LowMemoryNotification();
	//mainscope.Close();
    exitContext(context);
	v8::V8::TerminateExecution();
	CloseAllPlugins();
	CloseMessageBoxFunctions();
	TS_FreeLoadedScripts();
	free((void *)dir);
}

#ifdef _WIN32
int wmain
#else
int main
#endif
  (int argc, char* argv[]) {
    if(argc>1&&(strnlen(argv[1], 2)>0)&&((T5_IsDir(argv[1]))||(T5_IsFile(argv[1])))){
        printf(ENGINE " Info: We are running in given-path mode.\n");
        runGame(argv[1]);
    }
    else{
        runGame("startup/");
    }
}
