#ifndef FUNCTIONLOAD_HEAD
#define FUNCTIONLOAD_HEAD

#include <v8.h>

#include <TSPR/dlopenwrap.h>

//#define DLOPENFUNCTONPRESET(_type, _dlfunc, _handle, _name, _error, action) DLOPENFUNCTION(_type, _dlfunc, _handle, _name, "Error loading plugin : No function \"%s\".\n", _error, action)

typedef void** FunctionArray;

class regFunction {
public:
    regFunction(const char *, v8::Handle<v8::FunctionTemplate>, v8::Isolate *);
    regFunction(const char *_name, void (*func)(const v8::FunctionCallbackInfo<v8::Value>&), v8::Isolate *);
    ~regFunction();
    void registerToContext(v8::Handle<v8::Context> context, v8::Isolate *iso);
    const char *name;
    v8::Handle<v8::FunctionTemplate> templ;
};

//extern std::vector<regFunction> funclist;

void addFunctionToList(const char *name, void (*func)(const v8::FunctionCallbackInfo<v8::Value>&), v8::Isolate *iso);

void registerAllFunctions(v8::Handle<v8::Context> context, v8::Isolate *iso);

#endif