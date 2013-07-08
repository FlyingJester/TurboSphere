//#include "SDL_V8.h"
#include "engine.h"
#include "configmanager/openscript.h"
#include "configmanager/opengame.h"
#include <cstring>
#include "functionload.h"
#include "variableregister.h"
#include "loadplugins.h"

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
            printf("[Engine] RequireScript Info: Did not load script %s, same as script number %i.\n", scriptname, i);

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

void runGame(const char * dir){

    v8::HandleScope mainscope(v8::Isolate::GetCurrent());
    TS_Config *TS_conf = GetConfig();
    TS_Directories *TS_dirs = GetDirs();
    if(strnlen(dir, 2)>0){
        TS_dirs->root = STRDUP(string(dir).append("/").c_str());
        setDirectories(TS_dirs->root);
    }
    else{
        TS_dirs->root = "startup/";
        setDirectories(TS_dirs->root);
    }
    setConfig(TS_dirs->root);
    const char *gameSGMfile = STRDUP(string(TS_dirs->root).append("game.sgm").c_str());
    opengame(gameSGMfile);

    free((void *)gameSGMfile);

    printf("[Engine] The main script is %s\n", TS_conf->mainscript);

	std::string ScriptFileText = openfile(TS_conf->mainscript);

	const char *script_name = TS_conf->gamename;

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


    if(SDL_WasInit(SDL_INIT_EVERYTHING)==0){
        SDL_Init(SDL_INIT_TIMER);
        atexit(SDL_Quit);
    }
    else if(SDL_WasInit(SDL_INIT_TIMER)==0){
        SDL_InitSubSystem(SDL_INIT_TIMER);
    }

	ExecuteString(v8::String::New(ScriptFileText.c_str()), v8::String::New(script_name), true);

    TS_CallFunc("game", context);

    v8::V8::LowMemoryNotification();
    exitContext(context);
}

#ifdef _WIN32
int wmain
#else
int main
#endif
  (int argc, char* argv[]) {

    if(argc>1){
        runGame(argv[1]);
    }
    else{
        runGame("startup/");
    }
    return 0;
}
