//#include "SDL_V8.h"
#include "engine.h"
#include "configmanager/openscript.h"
#include "configmanager/opengame.h"
#include <cstring>
#include <cstdlib>
#include "functionload.h"
#include "variableregister.h"
#include "loadplugins.h"
#include "common/noreturn.h"
#include "v8-debug.h"
#ifdef _WIN32
#define STRDUP _strdup
#else
#define STRDUP strdup
#endif

//////////////////////////////////////////////////////////////
//Always update!
//////////////////////////////////////////////////////////////
#define VERSION "0.3.1"

static char ** loadedScripts = NULL;
static int numUniqueScriptsLoaded = 0;

void exitContext(v8::Persistent<v8::Context> context){
    context->v8::Context::Exit();
}



void TS_CallFunc(const char *name, v8::Persistent<v8::Context> context){
        v8::Handle<v8::Value> gameValue = context->Global()->Get(v8::String::New(name));
        if(false){
            printf("TS_CallGame error: game is not defined.");
            return;
        }
        if(!gameValue->IsFunction()){
            printf("TS_CallGame error: game is not a function.");
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
    v8::V8::LowMemoryNotification();
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
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, GetConfig()->gamename, *str, NULL);
    ExitGame(args);
}

v8::Handle<v8::Value> RequireScript(const v8::Arguments& args){
    if(args.Length()<1){
        return v8::ThrowException(v8::Exception::Error(v8::String::New("[Engine] RequireScript Error: Called with no arguments.")));
    }
    if(!args[0]->IsString()) {
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New("[Engine] RequireScript Error: Argument 0 is not a string.")));
    }
    v8::String::AsciiValue str(args[0]);
    const char *scriptname = *str;

    for(int i = 0; i< numUniqueScriptsLoaded; i++){
        if(strcmp(scriptname, loadedScripts[i])==0){
            fprintf(stderr, "[Engine] RequireScript Info: Did not load script %s, same as script number %i.\n", scriptname, i);

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
	return  v8::Number::New(SDL_GetTicks());
}

v8::Handle<v8::Value> Delay(const v8::Arguments& args)
{
	unsigned int t = args[0]->Uint32Value();
	SDL_Delay(t);
	return v8::Undefined();
}

void notationCallback(v8::GCType type, v8::GCCallbackFlags flags){
    printf("[Engine] GC was performed.\n");
}

//typedef void (*FatalErrorCallback)(const char* location, const char* message);

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
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, Name, str.c_str(), NULL);

}

void runGame(const char * path){

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
            printf("%s is the sgmname. We are running in / mode.\n", TS_conf->sgmname);
            gameSGMfile = STRDUP(std::string(dir).append(TS_conf->sgmname).c_str());
        }
        else {
            TS_dirs->root = STRDUP(std::string(dir).append("/").c_str());
            setDirectories(TS_dirs->root);
            setConfig(TS_dirs->root);
            printf("%s is the sgmname. We are running in NO / mode.\n", TS_conf->sgmname);
            gameSGMfile = STRDUP((std::string(TS_dirs->root)).append(TS_conf->sgmname).c_str());
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


        TS_dirs->root = STRDUP(std::string(dir).append("/").c_str());
        setDirectories(TS_dirs->root);
        setConfig(TS_dirs->root);
    }
    else{
        fprintf(stderr, "Invalid path '%s' given for the game.\n", path);
        free((void *)dir);
        exit(0);
    }

    v8::HandleScope mainscope(v8::Isolate::GetCurrent());

    opengame(gameSGMfile);

    free((void *)gameSGMfile);

    printf("[Engine] The main script is %s\n", TS_conf->mainscript);

	std::string ScriptFileText = openfile(TS_conf->mainscript);

	const char *script_name = TS_conf->mainscript;

    v8::Persistent<v8::Context> context(v8::Isolate::GetCurrent(), v8::Context::New(v8::Isolate::GetCurrent()));


    context->v8::Context::Enter();


	v8::HandleScope handle_scope2;

    //atexit(QuitAll);

    //init all plugins.
    loadAllPlugins();


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


    if(SDL_WasInit(SDL_INIT_EVERYTHING)==0){
        SDL_Init(SDL_INIT_TIMER);
        atexit(SDL_Quit);
    }
    else if(SDL_WasInit(SDL_INIT_TIMER)==0){
        SDL_InitSubSystem(SDL_INIT_TIMER);
    }

    v8::V8::SetCaptureStackTraceForUncaughtExceptions(true);

    v8::V8::AddMessageListener(TS_MessageCallback);

	ExecuteString(v8::String::New(ScriptFileText.c_str()), v8::String::New(script_name), true);

    TS_CallFunc("game", context);

    exitContext(context);
}

#ifdef _WIN32
int wmain
#else
int main
#endif
  (int argc, char* argv[]) {

    if(argc>1&&(strnlen(argv[1], 2)>0)){

        for(int i = 2; i<argc; i++){


        }
        runGame(argv[1]);
    }
    else{
        runGame("startup/");
    }
    return 0;
}
