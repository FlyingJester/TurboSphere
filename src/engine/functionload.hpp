#ifndef FUNCTIONLOAD_HEAD
#define FUNCTIONLOAD_HEAD

#include <v8.h>

#include <TSPR/dlopenwrap.h>

typedef void (*Function_)(const v8::FunctionCallbackInfo<v8::Value> &args);
typedef Function_* FunctionArray;
typedef const char ** NameArray;

class regFunction {
    std::string name;
    v8::Handle<v8::FunctionTemplate> templ;
public:
    regFunction(const std::string &name_, v8::Handle<v8::FunctionTemplate>, v8::Isolate *);
    regFunction(const std::string &name_, void (*func)(const v8::FunctionCallbackInfo<v8::Value>&), v8::Isolate *);
    void registerToContext(v8::Handle<v8::Context> context, v8::Isolate *iso) const;
};

void addFunctionToList(const std::string &name, void (*func)(const v8::FunctionCallbackInfo<v8::Value>&), v8::Isolate *iso);

void registerAllFunctions(v8::Handle<v8::Context> context, v8::Isolate *iso);

#endif
