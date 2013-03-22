#ifndef OPENSCRIPT_HEAD
#define OPENSCRIPT_HEAD
#include <string>
#include <vector>
using namespace std;

#ifdef _WIN32
	#ifdef CONFIGMGR_INTERNAL
		#define CONFIGMGR_EXPORT __declspec(dllexport) 
	#else
		#define CONFIGMGR_EXPORT __declspec(dllimport)
	#endif
#define CONFIGMGRCALL _cdecl
#else
	#define CONFIGMGR_EXPORT extern "C"
	#define CONFIGMGRCALL
#endif


CONFIGMGR_EXPORT std::string openfile(const char *Rfile);
CONFIGMGR_EXPORT bool ExecuteString(v8::Handle<v8::String> source,v8::Handle<v8::Value> name,bool print_result);
CONFIGMGR_EXPORT v8::Handle<v8::Value> TS_LoadScript(const v8::Arguments& args);
#endif
