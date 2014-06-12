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
CONFIGMGR_EXPORT bool ExecuteString(v8::Handle<v8::String> source, v8::Handle<v8::String> name, v8::Isolate *isolate, bool print_result);
CONFIGMGR_EXPORT void TS_LoadScript(const v8::FunctionCallbackInfo<v8::Value>& args);
CONFIGMGR_EXPORT void TS_LoadSystemScript(const v8::FunctionCallbackInfo<v8::Value>& args);
#endif
