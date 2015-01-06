#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <stdio.h>
#include <v8.h>
#include <t5_datasource.h>
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

char *openfile(const char *filename){

    std::unique_ptr<t5::DataSource> source (t5::DataSource::FromPath(t5::DataSource::eRead, filename));

    char *filetext = (char *)malloc(source->Length()+1);
    filetext[source->Length()] = '\0';

    source->Read(filetext, source->Length());

    return filetext;
}


bool ExecuteString(v8::Handle<v8::String> source,v8::Handle<v8::String> name, v8::Isolate *isolate, bool print_result) {

    if(!isolate){
        fprintf(stderr, "[ConfigManager] ExecuteString Error: isolate was null.");
        return false;
    }

    v8::ScriptOrigin origin(name);
    v8::Handle<v8::Script> script = v8::Script::Compile(source, &origin);


    if (script.IsEmpty()){
        v8::String::Utf8Value pname(name);

        printf("\nCould not compile script %s\n", *pname);
        return false;
    }
    else{
        v8::Handle<v8::Value> result = script->Run();
        if (result.IsEmpty()){
            printf("JS did not run successfully.\n");
            return false;
        }
        else{

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
    printf("[ConfigManager] Info: Loading script %s.\n", file.c_str());
    std::unique_ptr<char, void(*)(void *)> ScriptStr(openfile(file.c_str()), free);
    if(ScriptStr.get()[0]=='\0') {
        args.GetIsolate()->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(args.GetIsolate(), (std::string("[ConfigManager] ") + origin + " Error: Could not load script.").c_str())));
        return;
    }
    ExecuteString(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), ScriptStr.get()), v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), file.c_str()), v8::Isolate::GetCurrent(), false);
}


static void LoadScriptWithPrefix(const v8::FunctionCallbackInfo<v8::Value> &args, const char *origin, const char *prefix){
    if(args.Length()<1) {
        args.GetIsolate()->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(args.GetIsolate(), (std::string("[ConfigManager] ") + origin + " Error: No arguments given.\n").c_str())));
        return;
    }

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
