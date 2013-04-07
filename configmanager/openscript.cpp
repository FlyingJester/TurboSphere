#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include "../v8.h"
#define CONFIGMGR_INTERNAL
#include "openscript.h"
#include "opengame.h"
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
	printf("Opening script %s\n", Rfile);
	std::stringstream filetext;
	filetext.str("");

    string txt;
    ifstream file(Rfile);

    if(!file){
        printf("Could not open script: %s\n", Rfile);
    }
    else{
        //filetext<<" ";
    }

    if (file.is_open()){
		while (file.good()){
             getline(file, txt);

    filetext<<txt;
	filetext<<std::endl;
		}
	}
	else{
		printf("File not readable.\n");
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
    v8::String::AsciiValue error(try_catch.Exception());
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
      v8::String::AsciiValue error(try_catch.Exception());
      printf("%s\n", *error);
	printf("JS did not run successfully.\n");
      return false;
    }
	else
	{

      if (print_result && !result->IsUndefined())
	  {
        v8::String::AsciiValue str(result);
		printf("JS Result will be passed.\n");
		printf("%s\n", *str);
      }
    }
  }
  return true;
}

v8::Handle<v8::Value> TS_LoadScript(const v8::Arguments& args){
	TS_Directories *TS_dirs = GetDirs();
    if(args.Length()<1){
        return v8::ThrowException(v8::String::New("TS_LoadScript Error: No arguments given.\n"));
    }

    CHECK_ARG_STR(0, "TS_LoadScript Error: could not parse arg 0 to string.");

    v8::String::AsciiValue str(args[0]);
    const char *scriptname = *str;
    const char *scriptpath = STRDUP(string(TS_dirs->script).append(scriptname).c_str());
    string ScriptStr = openfile(scriptpath).c_str();
    if(ScriptStr.empty()){
        return v8::ThrowException(v8::String::New("TS_LoadScript Error: Could not load script."));
    }
    if(!ExecuteString(v8::String::New(ScriptStr.c_str()), v8::String::New(scriptname), true)){
		return v8::ThrowException(v8::String::New(((string)"Error in script "+string(scriptname)).c_str()));
	}
    return v8::Undefined();
}
