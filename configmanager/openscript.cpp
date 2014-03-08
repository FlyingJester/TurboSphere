#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include "../v8.h"
#define CONFIGMGR_INTERNAL
#include "openscript.h"
#include "opengame.h"

#define PLUGINNAME "configmanager"

#include "../plugins/common/plugin.h"
using namespace std;

#ifdef _WIN32
#define STRDUP _strdup
#else
#include <cstring>
#define STRDUP strdup
#endif

std::string openfile(const char *Rfile)
{
	std::stringstream filetext;
	filetext.str("");

    string txt;
    ifstream file(Rfile);

    if(!file){
        printf("[ConfigManager] Error: Could not open script %s\n", Rfile);
    }
    else{
        printf("[ConfigManager] Info: Successfully opened script %s\n", Rfile);
    }

    if (file.is_open()){
		while (file.good()){
             getline(file, txt);

    filetext<<txt;
	filetext<<std::endl;
		}
	}
	else{
		printf("[ConfigManager] Error: File not readable.\n");
	}
    file.close();



	std::string STR_filetext = filetext.str();

    return STR_filetext;
}


bool ExecuteString(v8::Handle<v8::String> source,v8::Handle<v8::Value> name,bool print_result)
{
  v8::TryCatch try_catch;
  v8::Handle<v8::Script> script = v8::Script::Compile(source, name);


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

void TS_LoadScript(const v8::FunctionCallbackInfo<v8::Value> &args){
	TS_Directories *TS_dirs = GetDirs();
    if(args.Length()<1){
        args.GetReturnValue().Set( v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "[ConfigManager] TS_LoadScript Error: No arguments given.\n")));
        return;
    }

    Turbo::CheckArg::String(args, 0, __func__);

    //CHECK_ARG_STR(0, "[ConfigManager] TS_LoadScript Error: could not parse arg 0 to string.");

    v8::String::Utf8Value str(args[0]);
    const char *scriptname = *str;
    const char *scriptpath = STRDUP(string(TS_dirs->script).append(scriptname).c_str());
    string ScriptStr = openfile(scriptpath).c_str();
    if(ScriptStr.empty()){
        args.GetReturnValue().Set( v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "[ConfigManager] TS_LoadScript Error: Could not load script.")));
        return;
    }
    if(!ExecuteString(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), ScriptStr.c_str()), v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), scriptname), true)){
		args.GetReturnValue().Set( v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), ((string)"Error in script "+string(scriptname)).c_str())));
		return;
	}
}

void TS_LoadSystemScript(const v8::FunctionCallbackInfo<v8::Value> &args){
	TS_Directories *TS_dirs = GetDirs();
    if(args.Length()<1){
        args.GetReturnValue().Set( v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "[ConfigManager] TS_LoadSystemScript Error: No arguments given.\n")));
        return;
    }


    Turbo::CheckArg::String(args, 0, __func__);

    //CHECK_ARG_STR(0, "[ConfigManager] TS_LoadSystemScript Error: could not parse arg 0 to string.");

    v8::String::Utf8Value str(args[0]);
    const char *scriptname = *str;
    const char *scriptpath = STRDUP(string(TS_dirs->systemscript).append(scriptname).c_str());
    string ScriptStr = openfile(scriptpath).c_str();
    if(ScriptStr.empty()){
        args.GetReturnValue().Set( v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "[ConfigManager] TS_LoadSystemScript Error: Could not load script.")));
        return;
    }
    if(!ExecuteString(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), ScriptStr.c_str()), v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), scriptname), true)){
		args.GetReturnValue().Set( v8::Exception::Error(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), ((string)"[ConfigManager] Error in script "+string(scriptname)).c_str())));
	}
}
