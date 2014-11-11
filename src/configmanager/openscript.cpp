#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <stdio.h>
#include <v8.h>
#define CONFIGMGR_INTERNAL
#include "openscript.h"
#include "opengame.h"

#define PLUGINNAME "configmanager"

#include <pluginsdk/plugin.h>
using namespace std;

#ifdef _WIN32
#define STRDUP _strdup
#else
#include <cstring>
#define STRDUP strdup
#endif

char *openfile(const char *Rfile){
    std::stringstream filetext;
    filetext.str("");

    string txt;
    ifstream file(Rfile);

    if(!file) {
        printf("[ConfigManager] Error: Could not open script %s\n", Rfile);
    }
    else {
        printf("[ConfigManager] Info: Successfully opened script %s\n", Rfile);
    }

    if (file.is_open()) {
        while (file.good()) {
            getline(file, txt);

            filetext<<txt;
            filetext<<std::endl;
        }
    }
    else {
        printf("[ConfigManager] Error: File not readable.\n");
    }
    file.close();


    return STRDUP(filetext.str().c_str());
}


bool ExecuteString(v8::Handle<v8::String> source,v8::Handle<v8::String> name, v8::Isolate *isolate ,bool print_result) {

    if(!isolate){
        fprintf(stderr, "[ConfigManager] ExecuteString Error: isolate was null.");
        return false;
    }

    v8::TryCatch try_catch;
    v8::ScriptOrigin origin(name);
    v8::Handle<v8::Script> script = v8::Script::Compile(source, &origin);


    if (script.IsEmpty())
    {
        v8::String::Utf8Value error(try_catch.Exception());
        v8::String::Utf8Value pname(name);
        string namestr = std::string(*pname);

        printf("%s\n", *error);
        printf("\nCould not compile script %s\n", namestr.c_str());
        return false;
    }
    else
    {
        v8::Handle<v8::Value> result = script->Run();
        if (result.IsEmpty())
        {
            v8::String::Utf8Value error(try_catch.Exception());
            printf("%s\n", *error);
            printf("JS did not run successfully.\n");
            return false;
        }
        else
        {

            if (print_result && !result->IsUndefined())
            {
                v8::String::Utf8Value str(result);
                printf("JS Result will be passed.\n");
                printf("%s\n", *str);
            }
        }
    }
    return true;
}

template<class T>
void OpenAndRunScript(const std::string &file, const char *origin, const T& args){
    std::unique_ptr<char, void(*)(void *)> ScriptStr(openfile(file.c_str()), free);
    if(ScriptStr.get()[0]=='\0') {
        args.GetReturnValue().Set( v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (std::string("[ConfigManager] ") + origin + " Error: Could not load script.").c_str())));
        return;
    }
    if(!ExecuteString(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), ScriptStr.get()), v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), file.c_str()), v8::Isolate::GetCurrent(), true)){
        args.GetReturnValue().Set( v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), ((string)"[ConfigManager] Error in script "+file).c_str())));
    }
}


static void LoadScriptWithPrefix(const v8::FunctionCallbackInfo<v8::Value> &args, const char *origin, const char *prefix){
    if(args.Length()<1) {
        args.GetReturnValue().Set( v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), (std::string("[ConfigManager] ") + origin + " Error: No arguments given.\n").c_str())));
        return;
    }

    v8::Isolate *isolate = v8::Isolate::GetCurrent();

    Turbo::CheckArg::String(args, 0, __func__);

    v8::String::Utf8Value str(args[0]);
    string scriptpath = string(prefix) + (*str);
    OpenAndRunScript(scriptpath, __func__, args);
}


void TS_LoadScript(const v8::FunctionCallbackInfo<v8::Value> &args){
    LoadScriptWithPrefix(args, __func__, GetDirs()->script);
}

void TS_LoadSystemScript(const v8::FunctionCallbackInfo<v8::Value> &args) {
    LoadScriptWithPrefix(args, __func__, GetDirs()->systemscript);
}
