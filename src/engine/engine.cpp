//TurboSphere - A plugin-based, JavaScript game engine.

#include "engine.h"
#include "version.h"
#include "options.h"

#include <openscript.h>
#include <opengame.h>
#include <screen.h>

#include "functionload.h"
#include "variableregister.h"
#include "loadplugins.h"
#include "typed_array.h"

#include <TSPR/noreturn.h>
#include "platform/time.hpp"
#include "platform/platform.hpp"

#if defined(HAS_UNISTD)
#include <unistd.h>
#elif defined(HAS_UNISTD_SYS)
#include <sys/unistd.h>
#endif

#include <t5.h>

#include <cstring>
#include <cctype>
#include <utility>

using std::pair;

#define ENGINE "[Engine]"

#ifdef _WIN32
#define STRDUP _strdup
#else
#define STRDUP strdup
#endif

void (*TS_MessageBox)(const char * title, const char *content);
int (*SDL_ShowSimpleMessageBox)(uint32_t flags, const char *title, const char *message, void *window);
void (*TS_Sleep)(uint32_t milliseconds);
uint32_t (*TS_GetTime)(void);

#include "platform/time.hpp"

#if ( defined __MACH__ ) && (defined __APPLE__ )

    #define OS_X 1
#endif

inline void TS_OverrideConfig(TS_Config *conf, TS_ConfigOverride *overrideConf){
    TS_Config *oconf = overrideConf->config;
    if(overrideConf->gamefunc){
        conf->gamefunc = oconf->gamefunc;
    }
	if(overrideConf->sgmname)
        conf->sgmname = oconf->sgmname;

	if(overrideConf->gamename)
        conf->gamename = oconf->gamename;

	if(overrideConf->mainscript)
        conf->mainscript = oconf->mainscript;

	if(overrideConf->screenwidth){
        conf->screenwidth = oconf->screenwidth;
        SetScreenWidth(oconf->screenwidth);
	}
	if(overrideConf->screenheight){
        conf->screenheight = oconf->screenheight;
        SetScreenHeight(oconf->screenheight);
	}
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

enum TS_ConfigType {TS_Int, TS_Float, TS_String, TS_Bool};
typedef pair<const char *, TS_ConfigType> TS_ConfVal;

#define CHECK_PARAM(n, p) (strncmp(n, p, strnlen(n, 0xFF))==0)

#define CUT_OUT_BLANK(p) while((*p!=' ')&&(*p!='=')&&(p[1]!='\0')) p++; p++;
#define SET_PARAM_INT(n, p, c) if(CHECK_PARAM(#n, p)) {CUT_OUT_BLANK(p); c.config->n = atoi(p); c.n=true; printf("[Engine] Info: Overrode " #n " in config with %s.\n", p); return;}
#define SET_PARAM_BOOL(n, p, c) if(CHECK_PARAM(#n, p)) {CUT_OUT_BLANK(p); c.config->n = (atoi(p)||false); c.n=true; printf("[Engine] Info: Overrode " #n " in config with %i.\n", atoi(p)); return;}
#define SET_PARAM_STR(n, p, c) if(CHECK_PARAM(#n, p)) {CUT_OUT_BLANK(p); c.config->n = STRDUP(p); c.n=true; printf("[Engine] Info: Overrode " #n " in config with %i.\n", (p?1:0)); return;}


void TS_SetConfigOverrideFromParameter(const char *param, TS_ConfigOverride &conf){
    /*
    static const TS_ConfVal flags[] = {
        TS_ConfVal("gamename", TS_String),
        TS_ConfVal("sgmname",TS_String),
        TS_ConfVal("gamefunc",TS_String),
        TS_ConfVal("mainscript",TS_String),
        TS_ConfVal("screenwidth", TS_Int), TS_ConfVal("screenheight", TS_Int),
        TS_ConfVal("fullscreen", TS_Bool),
        TS_ConfVal("compositing", TS_Bool),
        TS_ConfVal("scale",TS_Int),
        TS_ConfVal("systemfont",TS_String), TS_ConfVal("systemttffont",TS_String),
        TS_ConfVal("systemwindowstyle",TS_String),
        TS_ConfVal("systemarrow",TS_String),
        TS_ConfVal("systemuparrow",TS_String),
        TS_ConfVal("systemdownarrow",TS_String),
        TS_ConfVal("systemsoundfont",TS_String),
        TS_ConfVal("fixedplugins",TS_Int)
    };
    */
    printf("[Engine] Info: Checking arg %s.\n", param);

    const char *paramStart = param;
    while(*paramStart=='-')
        paramStart++;

    printf("[Engine] Info: Checking arg %s.\n", paramStart);



    SET_PARAM_STR(gamename, paramStart, conf);
    SET_PARAM_STR(sgmname, paramStart, conf);
    SET_PARAM_STR(gamefunc, paramStart, conf);
    SET_PARAM_STR(mainscript, paramStart, conf);
    SET_PARAM_INT(screenwidth, paramStart, conf);
    SET_PARAM_INT(screenheight, paramStart, conf);
    SET_PARAM_BOOL(compositing, paramStart, conf);
    SET_PARAM_BOOL(fullscreen, paramStart, conf);
    SET_PARAM_INT(scale, paramStart, conf);
    SET_PARAM_STR(systemfont, paramStart, conf);
    SET_PARAM_STR(systemttffont, paramStart, conf);
    SET_PARAM_STR(systemarrow, paramStart, conf);
    SET_PARAM_STR(systemuparrow, paramStart, conf);
    SET_PARAM_STR(systemdownarrow, paramStart, conf);
    SET_PARAM_STR(systemsoundfont, paramStart, conf);

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

TS_ConfigOverride::~TS_ConfigOverride(){
}
void TS_SDLMessageBox(const char *title, const char *content){
    SDL_ShowSimpleMessageBox(0x00000040, title, content, NULL);
}

void TS_TerminalMessageBox(const char *title, const char *content){
    printf(ENGINE " Info: %s\n", title);
    printf("\t%s\n", content);
}

void * SDLhandle = NULL;

#ifdef OS_X
#include "platform/osx_messagebox.h"
#endif

void LoadMessageBoxFunctions(void){
    SDLhandle = dlopen("libSDL2.so", RTLD_GLOBAL|RTLD_NOW);
    if(SDLhandle!=NULL){
        SDL_ShowSimpleMessageBox = (int(*)(uint32_t, const char *, const char *, void *))dlsym(SDLhandle, "SDL_ShowSimpleMessageBox");
        TS_MessageBox = TS_SDLMessageBox;
        TS_Sleep = (void(*)(uint32_t))dlsym(SDLhandle, "SDL_Delay");
        TS_GetTime = (uint32_t(*)(void))dlsym(SDLhandle, "SDL_GetTicks");
    }

    if(SDL_ShowSimpleMessageBox==NULL){
        #ifdef OS_X
          TS_MessageBox = TS_OSX_MessageBox;
          return;
        #endif

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
static char ** loadedSystemScripts = NULL;
static int numUniqueScriptsLoaded = 0;
static int numUniqueSystemScriptsLoaded = 0;

template<char ***ScriptArray, int *Num>
void TS_FreeLoadedScripts(void){
    for(int i = 0; i<(*Num); i++){
        free((void *)(*ScriptArray)[i]);
        (*ScriptArray)[i] = NULL;
    }
    free(*ScriptArray);
    (*ScriptArray) = NULL;
    (*Num) = 0;
}

void exitContext(v8::Handle<v8::Context> context){
    //ncontext.Reset();
}

void TS_CallFunc(const char *name, v8::Handle<v8::Context> context, v8::Isolate *iso){
        v8::Handle<v8::Value> gameValue = context->Global()->Get(v8::String::NewFromUtf8(iso, name));
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


void GetVersionNumber(const v8::FunctionCallbackInfo<v8::Value> &args){
    args.GetReturnValue().Set( v8::Number::New(v8::Isolate::GetCurrent(), 2.0));
}

void GetVersionString(const v8::FunctionCallbackInfo<v8::Value> &args){
    args.GetReturnValue().Set( v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), TS_GetVersion()));
}

void GarbageCollect(const v8::FunctionCallbackInfo<v8::Value> &args){
  /*
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
*/
}

TS_NORETURN void ExitGame(const v8::FunctionCallbackInfo<v8::Value> &args){
    exit(0);
}

TS_NORETURN void AbortGame(const v8::FunctionCallbackInfo<v8::Value> &args){
    if(args.Length()<1){
        ExitGame(args);
    }
    v8::String::Utf8Value str(args[0]);
    TS_MessageBox(GetConfig()->gamename, *str);
    ExitGame(args);
}

template<void (*LoadScript)(const v8::FunctionCallbackInfo<v8::Value>& args), char ***ScriptList, int *NumScripts>
void RequireScript(const v8::FunctionCallbackInfo<v8::Value> &args){
    if(args.Length()<1){
        args.GetReturnValue().Set( v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), ENGINE " RequireScript Error: Called with no arguments.")));
        return;
    }
    if(!args[0]->IsString()) {
        args.GetReturnValue().Set( v8::Exception::TypeError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), ENGINE " RequireScript Error: Argument 0 is not a string.")));
        return;
    }
    v8::String::Utf8Value str(args[0]);


    for(int i = 0; i< (*NumScripts); i++){
        if(strcmp(*str, (*ScriptList)[i])==0){
            return;
        }
    }

    (*NumScripts)++;
    *ScriptList = (char **)realloc(*ScriptList, (*NumScripts)*(sizeof(char *)));

    (*ScriptList)[(*NumScripts)-1] = STRDUP(*str);

    LoadScript(args);
    return;
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

    str+= "\n\nStack Trace:\n";

    v8::Local<v8::StackTrace> trace = message->GetStackTrace();

    for(int i = 0; i<trace->GetFrameCount(); i++){
        v8::Local<v8::StackFrame> frame = trace->GetFrame(i);
        str+= *(v8::String::Utf8Value(frame->GetFunctionName()));

        {
            char lLineNumCharl[LINE_NUM_CHAR_SIZE+1] = {LINE_NUM_FILL};
            snprintf(lLineNumCharl, LINE_NUM_CHAR_SIZE, "%6i", frame->GetLineNumber());
            char *lLineNumChar = lLineNumCharl;
            lLineNumChar = destructiveTrim(lLineNumChar);
            str+=", line ";
            str+= lLineNumChar;
        }
        str+="\n";
    }

    fprintf(stderr, "%s\n", str.c_str());
    TS_MessageBox(Name, str.c_str());

}

void runGame(const char * path, const char *v8Flags, TS_ConfigOverride *overrideConf){

    if(v8Flags!=NULL){
        printf("[Engine] Info: Using v8 flags %s\n", v8Flags);
        v8::V8::SetFlagsFromString(v8Flags, strlen(v8Flags));
    }
    else{

        printf("[Engine] Info: Using no v8 flags.\n");

    }

    char * dir;

    const char *gameSGMfile;

    TS_Config *TS_conf = GetConfig();
    TS_Directories *TS_dirs = GetDirs();

    dir = STRDUP(path);
    size_t path_size_chr = strlen(dir);

    if(t5::IsDir(path)){

        if(dir[path_size_chr-1]=='/'){
            TS_dirs->root = STRDUP(dir);
            setDirectories(TS_dirs->root);
            setConfig(TS_dirs->root);

            if((overrideConf!=NULL)&&(overrideConf->config!=NULL)){
                TS_OverrideConfig(TS_conf, overrideConf);
            }

            printf(ENGINE " Info: %s is the sgmname. We are running in / mode.\n", TS_conf->sgmname);
            gameSGMfile = STRDUP(string(dir).append(TS_conf->sgmname).c_str());
        }
        else {
            TS_dirs->root = STRDUP(string(dir).append("/").c_str());
            setDirectories(TS_dirs->root);
            setConfig(TS_dirs->root);

            if((overrideConf!=NULL)&&(overrideConf->config!=NULL)){
                TS_OverrideConfig(TS_conf, overrideConf);
            }

            printf(ENGINE " Info: %s is the sgmname. We are running in NO / mode.\n", TS_conf->sgmname);
            gameSGMfile = STRDUP((string(TS_dirs->root)).append(TS_conf->sgmname).c_str());
        }
    }
    else if (t5::IsFile(path)){

        //If the path ends in a slash, we need to remove it so that we can use just the filename.

        //This stops crashes with the FLTK+T5 launcher when it is set to use directories, and then
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
        if((overrideConf!=NULL)&&(overrideConf->config!=NULL)){
            TS_OverrideConfig(TS_conf, overrideConf);
        }
    }
    else{
        if(stderr==stdin)
            fprintf(stderr, ENGINE " Error: Invalid path '%s' given for the game.\n", path);
        else
            fprintf(stderr, "[Engine] Error: Invalid path '%s' given for the game.\n", path);

        free((void *)dir);
        exit(0);
    }

    v8::V8::Initialize();

    LoadMessageBoxFunctions();

    v8::Isolate *iso = v8::Isolate::New();
    if(!iso){
      printf("[Engine] Fatal Error: Could not initialize Isolate.\n");
      return;
    }
    else
      printf("[Engine] Info: Created Isolate.\n");
    v8::Isolate::Scope isolateScope(iso);
    v8::HandleScope mainScope(iso);

    v8::Local<v8::Context> context = v8::Context::New(iso);
    printf("[Engine] Info: Created engine context.\n");

    context->Enter();

    opengame(gameSGMfile);
    free((void *)gameSGMfile);
    if((TS_conf->mainscript==NULL)||TS_conf->mainscript[0]=='\0'){
        printf("\n");
        exit(10);

    }

    if((overrideConf!=NULL)&&(overrideConf->config!=NULL)){
        TS_OverrideConfig(TS_conf, overrideConf);
    }
    setDirectories(TS_dirs->root);

    if((overrideConf!=NULL)&&(overrideConf->config!=NULL)){
        TS_OverrideConfig(TS_conf, overrideConf);
    }

    loadAllPlugins();
    v8::V8::AddMessageListener(TS_MessageCallback);

    printf(ENGINE " Info: All Plugins initialized.\n");
    printf(ENGINE " Info: The main script is %s\n", TS_conf->mainscript);
    printf(ENGINE " Info: Running game %s\n",       TS_conf->gamename);

	std::string ScriptFileText = openfile(TS_conf->mainscript);

	const char *script_name = TS_conf->mainscript;

//    v8::V8::InitializeICU();


    iso->Enter();

    printf("[Engine] info Isolate created at %p\n", iso);

    v8::HandleScope scope(iso);
    v8::V8::SetCaptureStackTraceForUncaughtExceptions(true);

    v8::ArrayBuffer::Allocator* allocator = new Turbo::ArrayBufferAllocator();

    v8::V8::SetArrayBufferAllocator(allocator);

    registerAllFunctions(context, iso);
    registerAllVariables(context, iso);

    v8::Handle<v8::FunctionTemplate> E_EvaluateScripttempl = v8::FunctionTemplate::New(iso, TS_LoadScript);

    v8::Handle<v8::FunctionTemplate> E_RequireScripttempl = v8::FunctionTemplate::New(iso, RequireScript<TS_LoadScript, &loadedScripts, &numUniqueScriptsLoaded>);

    v8::Handle<v8::FunctionTemplate> E_RequireSystemScripttempl = v8::FunctionTemplate::New(iso, RequireScript<TS_LoadSystemScript, &loadedSystemScripts, &numUniqueSystemScriptsLoaded>);

    v8::Handle<v8::FunctionTemplate> E_GetVersionNumbertempl = v8::FunctionTemplate::New(iso, GetVersionNumber);

    v8::Handle<v8::FunctionTemplate> E_GetVersionStringtempl = v8::FunctionTemplate::New(iso, GetVersionString);

    v8::Handle<v8::FunctionTemplate> E_GetTimetempl = v8::FunctionTemplate::New(iso, Turbo::GetTime);

    v8::Handle<v8::FunctionTemplate> E_GetSecondstempl = v8::FunctionTemplate::New(iso, Turbo::GetSeconds);

    v8::Handle<v8::FunctionTemplate> E_Delaytempl = v8::FunctionTemplate::New(iso, Turbo::Delay);

    v8::Handle<v8::FunctionTemplate> E_Exittempl = v8::FunctionTemplate::New(iso, ExitGame);

    v8::Handle<v8::FunctionTemplate> E_Aborttempl = v8::FunctionTemplate::New(iso, AbortGame);

    v8::Handle<v8::FunctionTemplate> E_GarbageCollecttempl = v8::FunctionTemplate::New(iso, GarbageCollect);

    context->Global()->Set(v8::String::NewFromUtf8(iso, "EvaulateScript"), E_EvaluateScripttempl->GetFunction());

    context->Global()->Set(v8::String::NewFromUtf8(iso, "RequireScript"), E_RequireScripttempl->GetFunction());

    context->Global()->Set(v8::String::NewFromUtf8(iso, "RequireSystemScript"), E_RequireSystemScripttempl->GetFunction());

    context->Global()->Set(v8::String::NewFromUtf8(iso, "GetVersion"), E_GetVersionNumbertempl->GetFunction());

    context->Global()->Set(v8::String::NewFromUtf8(iso, "GetVersionString"), E_GetVersionStringtempl->GetFunction());

    context->Global()->Set(v8::String::NewFromUtf8(iso, "GetTime"), E_GetTimetempl->GetFunction());

    context->Global()->Set(v8::String::NewFromUtf8(iso, "GetSeconds"), E_GetSecondstempl->GetFunction());

    context->Global()->Set(v8::String::NewFromUtf8(iso, "Delay"), E_Delaytempl->GetFunction());

    context->Global()->Set(v8::String::NewFromUtf8(iso, "Exit"), E_Exittempl->GetFunction());

    context->Global()->Set(v8::String::NewFromUtf8(iso, "Abort"), E_Aborttempl->GetFunction());

    context->Global()->Set(v8::String::NewFromUtf8(iso, "GarbageCollect"), E_GarbageCollecttempl->GetFunction());

    {

        v8::TryCatch try_catch;

        v8::Local<v8::String> name = v8::String::NewFromUtf8(iso, script_name);
        v8::ScriptOrigin origin(name);
        v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::NewFromUtf8(iso, ScriptFileText.c_str()), &origin);

        v8::Handle<v8::Value> result = script->Run();
        if (result.IsEmpty()){
            v8::String::Utf8Value error(try_catch.Exception());
            printf("%s\n", *error);
            printf("JS did not run successfully.\n");
            TS_MessageCallback(try_catch.Message(), v8::String::NewFromUtf8(iso, script_name));
            goto end;
        }

    }

    TS_CallFunc("game", context, iso);

	printf(ENGINE " Info: Game function done running.\n");

end:

    exitContext(context);

	v8::V8::Dispose();
	CloseAllPlugins();
	CloseMessageBoxFunctions();
	TS_FreeLoadedScripts<&loadedScripts, &numUniqueScriptsLoaded>();
	TS_FreeLoadedScripts<&loadedSystemScripts, &numUniqueSystemScriptsLoaded>();
	free((void *)dir);
	delete allocator;
}

#ifdef _WIN32
int wmain
#else
int main
#endif
(int argc, char* argv[]){

    std::unique_ptr<v8::Platform> platform(new Turbo::Platform());

    v8::V8::InitializePlatform(platform.get());

    TS_CMD_ProcessStubbingOptions(argc, argv);

    const char * flagprefix = " --";

    char *v8flags = (char *)malloc(1);

    *v8flags = '\0';

    int v8args = 0;

    int gamearg = 1;
    TS_ConfigOverride conf;
    conf.config = (TS_Config*)malloc(sizeof(TS_Config));

    printf("TurboSphere %s\n", TS_GetLongVersion());

    if(argc<3)
        gamearg = 1;
    else{
        for(int i = 1; i<argc; i++){
            if((argv[i][0]=='-')&&(argv[i][1]=='-')){

                if(((argv[i][2]=='V')||(argv[i][2]=='v'))&&(argv[i][3]=='8')){

                    if(strlen(argv[i])<=4)
                        continue;

                    size_t oldsize = strlen(v8flags);
                                                                    //-4 for "--v8", +3 for " --" at the start, +1 for NULL.
                    v8flags = (char *)realloc(v8flags, (oldsize+strlen(argv[i])));
                    strcat(v8flags, flagprefix);
                    strcat(v8flags, argv[i]+4);
                    v8args++;
                }
                else if((t5::IsDir(argv[i]))||(t5::IsFile(argv[i]))){
                    gamearg = i;
                }
                else{
                    if(i==1)
                        continue;

                    TS_SetConfigOverrideFromParameter(argv[i], conf);
                }
            }
        }
    }

    if(argc>1&&(strnlen(argv[gamearg], 2)>0)&&((t5::IsDir(argv[gamearg]))||(t5::IsFile(argv[gamearg])))){
        printf(ENGINE " Info: We are running in given-path mode.\n");
        runGame(argv[1], (v8args)?v8flags:NULL, &conf);
    }
    else{
        /*
        TS_ConfigOverride conf2;
        for(int i = 1; i<argc; i++){
            if((argv[i][0]=='-')&&(argv[i][1]=='-')){
//                TS_SetConfigOverrideFromParameter(argv[i], conf2);

                if(((argv[i][2]=='V')||(argv[i][2]=='v'))&&(argv[i][3]=='8')){
                */
                /*
                    size_t oldsize = strlen(v8flags);
                                                                    //-4 for "--v8", +3 for " --" at the start.
                    v8flags = (char *)realloc(v8flags, (oldsize+strlen(argv[i])-1));
                    strcat(v8flags, flagprefix);
                    strcat(v8flags, argv[i]+4);
                    v8args++;*/
                    /*
                }
                else{
                    TS_SetConfigOverrideFromParameter(argv[i], conf2);
                }
            }
        }*/
        char *lv8args =TS_CMD_ProcessV8Options(argc, argv);
        if(lv8args!=nullptr)
          runGame("startup/", lv8args, &conf);
        else
          runGame("startup/", (v8args)?v8flags:NULL, &conf);
    }

    free(v8flags);

    free(conf.config);
}
