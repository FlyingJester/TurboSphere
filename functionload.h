#ifndef FUNCTIONLOAD_HEAD
#define FUNCTIONLOAD_HEAD

#include "v8.h"

#ifdef _MSC_VER
#include <windows.h>
#define DLOPENFUNCTION(_type, _dlfunc, _handle, _name, _error, action)\
    _dlfunc = (_type)GetProcAddress(_handle, _name);\
    if ((_dlfunc) == NULL)  {\
        printf("Error loading plugin : No function \"%s\".\n", _name);\
        action;\
    }

#elif defined __GNUC__

#include <stdlib.h>
#include <dlfcn.h>

#define DLOPENFUNCTION(_type, _dlfunc, _handle, _name, _error, action)\
    _dlfunc = (_type)dlsym(_handle, _name);\
    if ((_error = dlerror()) != NULL)  {\
        fprintf (stderr, "%s\n", error);\
        printf("Error loading plugin : No function \"%s\".\n", _name);\
        action;\
    }

#endif

typedef void** FunctionArray;

class regFunction {
public:
    regFunction(const char *, v8::Handle<v8::FunctionTemplate>);
    regFunction(const char *name, v8::Handle<v8::Value> (*func)(const v8::Arguments& args));
    ~regFunction();
    void registerToContext(v8::Persistent<v8::Context> context);
    const char *name;
    v8::Handle<v8::FunctionTemplate> templ;
};

//extern std::vector<regFunction> funclist;

void addFunctionToList(const char *name, v8::Handle<v8::Value> (*func)(const v8::Arguments& args));

void registerAllFunctions(v8::Persistent<v8::Context> context);

#endif
