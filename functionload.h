#ifndef FUNCTIONLOAD_HEAD
#define FUNCTIONLOAD_HEAD

#include "v8.h"

#include "common/dlopenwrap.h"

#define DLOPENFUNCTONPRESET(_type, _dlfunc, _handle, _name, _error, action) DLOPENFUNCTION(_type, _dlfunc, _handle, _name, "Error loading plugin : No function \"%s\".\n", _error, action)

typedef void** FunctionArray;

class regFunction {
public:
    regFunction(const char *, v8::Handle<v8::FunctionTemplate>);
    regFunction(const char *_name, v8::Handle<v8::Value> (*func)(const v8::Arguments& args));
    ~regFunction();
    void registerToContext(v8::Persistent<v8::Context> context);
    const char *name;
    v8::Handle<v8::FunctionTemplate> templ;
};

//extern std::vector<regFunction> funclist;

void addFunctionToList(const char *name, v8::Handle<v8::Value> (*func)(const v8::Arguments& args));

void registerAllFunctions(v8::Persistent<v8::Context> context);

#endif
